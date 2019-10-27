#include <boost/program_options/positional_options.hpp>
#include <boost/program_options.hpp>
#include "utils.h"
#include <unistd.h>
#include <sys/wait.h>


char ** vecstr2char ( VecStr vecStr )
{
    auto argv = new char * [vecStr.size()];
    for ( size_t i = 0; i < vecStr.size(); ++i )
        argv[i] = ( char * ) ( vecStr[i].c_str());
    return argv;
}


std::vector<std::string> parse_line ( const std::string & commandline )
{
    bool quotes_are_opened {false};
    std::vector<std::string> args {};
    std::string buff {};
    for ( auto & c: commandline )
    {
        if ( c == '"' ) quotes_are_opened ^= static_cast<unsigned>(1);

        if (( c == ' ' ) && ( !quotes_are_opened ))
        {
            args.push_back(buff);
            buff.clear();
            continue;
        }
        buff += c;
    }
    args.push_back(buff);
    return args;
}


void parse_arguments ( const VecStr & vecStr, Args & args )
{
    auto argv = vecstr2char(vecStr);
    cout << argv[0] << endl;
    cout << argv[1] << endl;
    cout << "size: " << vecStr.size() << endl;
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen")
                ("path", po::value<std::string>(), "for cd")
                ("exit_code", po::value<int>(), "for exit")
                ("words", po::value<std::vector<std::string>>()->
                        multitoken()->zero_tokens()->composing(), "for echo")
                ("var_name", po::value<std::string>(), "for export");

        po::positional_options_description pos_desc;
        pos_desc.add("path", -1);
        pos_desc.add("exit_code", -1);
        pos_desc.add("words", -1);
        pos_desc.add("var_name", -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        parser.options(desc).positional(pos_desc).allow_unregistered();
        po::parsed_options parsed_options = parser.run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        if ( vm.count("help"))
            args.help = true;

        if ( vm.count("path"))
            args.path = vm["path"].as<std::string>();

        if ( vm.count("exit_code"))
            args.exit_code = vm["exit_code"].as<int>();

        if ( vm.count("words"))
            args.words = vm["words"].as<VecStr>();

        if ( vm.count("var_name"))
            args.var_name = vm["var_name"].as<std::string>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );

}

void run_if_in_path(const std::vector<std::string> &command_opts){
    auto command_name = command_opts[0];
    for (auto &loc: PATH){
        if ( boost::filesystem::exists(loc + command_name) & ( !access(( loc + command_name ).c_str(), X_OK)))
        {
            fork_exec(loc+command_name, command_opts);
            return;
        }
    }
    if ( !access(command_name.c_str(), X_OK))
    {
        fork_exec(command_name, command_opts);
        return;
    }
    throw std::runtime_error("No such command exists");
}

void fork_exec(const std::string & exec_name, const VecStr & arguments){

    pid_t parent = getpid();
    pid_t pid = fork();


    if (pid == -1)
    {
        throw std::runtime_error("Failed to fork()");
    }
    else if (pid > 0)
    {

        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        //        for (auto & a: arguments)
        //            cout << a << endl;
        //        cout << "filename: " << fs::path{exec_name}.filename() << endl;
        auto exec_name_name = new char[fs::path {exec_name}.filename().size()];
        strcpy(exec_name_name, fs::path {exec_name}.filename().c_str());
        char * argm[] = {exec_name_name, NULL};
        char * env[] = {NULL};
        execve(exec_name.c_str(), argm, env);
        std::flush(std::cout);
    }
}
