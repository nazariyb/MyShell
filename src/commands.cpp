#include "commands.h"
#include "utils.h"

//#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options.hpp>
//#include <fcntl.h>
#include <iostream>
//#include <fstream>


bool variable_not_found {false};

void Command::log ( const std::string & message, OUT_TYPE out_type, bool is_end )
{
    log(message, out_type, is_end, "");
}


void Command::log ( const std::string & message, OUT_TYPE out_type, bool is_end, const std::string & dest )
{

    if ( dest.empty())
    {
        ( out_type == ERROR ? std::cerr : std::cout ) << message;
        return;
    }

    if ( !out_file.is_open())
        out_file.open(dest, std::ios::out);

    out_file << message;

    if ( is_end )
        out_file.close();

}


void Command::parse_redirections ( VecStr & vecStr, Args & args )
{
    static std::map<std::string, std::string *> arrows2args
            {
                    {"<",  &args.input},
                    {">",  &args.output},
                    {"2>", &args.error_output}
            };

    static std::map<std::string, const std::string *> stdioe
            {
                    {"&0", &args.input},
                    {"&1", &args.output},
                    {"&2", &args.error_output}
            };


    VecStr new_vec_str {};
    for ( size_t i = 0; i < vecStr.size(); ++i )
    {
        if ( arrows2args.count(vecStr[i]))
        {

            if ( vecStr[i] == "<" )
                args.input = ( stdioe.count(vecStr[i + 1])) ? *( stdioe[vecStr[i + 1]] ) : vecStr[i + 1];
            else if ( vecStr[i] == ">" )
                args.output = ( stdioe.count(vecStr[i + 1])) ? *( stdioe[vecStr[i + 1]] ) : vecStr[i + 1];
            else if ( vecStr[i] == "2>" )
                args.error_output = ( stdioe.count(vecStr[i + 1])) ? *( stdioe[vecStr[i + 1]] ) : vecStr[i + 1];

            ++i;
        }
        else
            new_vec_str.emplace_back(vecStr[i]);
    }

    vecStr = new_vec_str;
}


Args Command::base_parsing ( VecStr & vecStr, od & desc, po::variables_map & vm, const VecStr & pos_args )
{
    Args args {};
    parse_redirections(vecStr, args);

    auto argv = vecstr2char(vecStr, false);
    try
    {
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen");


        po::positional_options_description pos_desc;
        for ( auto & pos_arg: pos_args )
            pos_desc.add(pos_arg.c_str(), -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .run();

        store(parsed_options, vm);
        notify(vm);

        args.help = vm["help"].as<bool>();
    }
    catch ( const po::error & ex )
    {
        log(std::string {ex.what()} + '\n',
            OUT_TYPE::ERROR, true, args.error_output);
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mErrno::parse_arguments ( VecStr & vecStr )
{
    od desc {"Options"};
    po::variables_map vm;
    return base_parsing(vecStr, desc, vm, {});
}


Args mPwd::parse_arguments ( VecStr & vecStr )
{
    od desc {"Options"};
    po::variables_map vm;
    return base_parsing(vecStr, desc, vm, {});
}


Args mCd::parse_arguments ( VecStr & vecStr )
{
    od desc {"Options"};
    desc.add_options()
            ("path", po::value<std::string>(), "Help screen");

    po::variables_map vm;
    auto args = base_parsing(vecStr, desc, vm, {"path"});

    if ( !vm.count("path")) throw std::invalid_argument("path is not specified!");
    args.path = vm["path"].as<std::string>();

    return args;
}


Args mExit::parse_arguments ( VecStr & vecStr )
{
    od desc {"Options"};
    desc.add_options()
            ("exit_code", po::value<int>(), "Help screen");

    po::variables_map vm;
    auto args = base_parsing(vecStr, desc, vm, {"exit_code"});

    if ( vm.count("exit_code"))
        args.exit_code = vm["exit_code"].as<int>();

    return args;
}


Args mEcho::parse_arguments ( VecStr & vecStr )
{
    od desc {"Options"};
    desc.add_options()
            ("words", po::value<std::vector<std::string>>()->
                    multitoken()->zero_tokens()->composing(), "Help screen");

    po::variables_map vm;
    auto args = base_parsing(vecStr, desc, vm, {"words"});

    if ( vm.count("words"))
        args.words = vm["words"].as<VecStr>();

    return args;
}


Args mExport::parse_arguments ( VecStr & vecStr )
{
    od desc {"Options"};
    desc.add_options()
            ("var_name", po::value<std::string>(), "Help screen");

    po::variables_map vm;
    auto args = base_parsing(vecStr, desc, vm, {"var_name"});

    if ( vm.count("var_name"))
        args.var_name = vm["var_name"].as<std::string>();

    return args;
}


Args mDot::parse_arguments ( VecStr & vecStr )
{
    od desc {"Options"};
    desc.add_options()
            ("script_name", po::value<std::string>(), "Help screen");

    po::variables_map vm;
    auto args = base_parsing(vecStr, desc, vm, {"script_name"});

    if ( vm.count("script_name"))
        args.var_name = vm["script_name"].as<std::string>();

    return args;
}

EXIT_CODE mErrno::run ( VecStr & parsed_line )
{
    Args args {};
    try
    {
        args = parse_arguments(parsed_line);
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }

    if ( args.help )
    {
        log("merrno [-h|--help] – вивести код завершення останньої програми чи команди\n"
            "Повертає нуль, якщо ще жодна програма не виконувалася.\n"
            "Після неї самої merrno повертає нуль, крім випадку, коли було передано невірні опції.\n",
            OUT_TYPE::OUT, true, args.output);
        return HELP_EXIT;
    }
    log(std::to_string(last_exit_code) + "\n",
        OUT_TYPE::OUT, true, args.output);
    return SUCCESS;
}


EXIT_CODE mPwd::run ( VecStr & parsed_line )
{
    Args args {};
    try
    {
        args = parse_arguments(parsed_line);
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }

    if ( args.help )
    {
        log("mpwd [-h|--help] – вивести поточний шлях\n"
            "Після неї merrno повертає нуль, крім випадку, коли було передано невірні опції.\n",
            OUT_TYPE::OUT, true, args.output);
        return HELP_EXIT;
    }
    log(fs::current_path().string() + "\n", OUT_TYPE::OUT, true, args.output);
    return SUCCESS;
}


EXIT_CODE mCd::run ( VecStr & parsed_line )
{
    Args args {};
    try
    {
        args = parse_arguments(parsed_line);
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }

    try
    {
        auto value = get_variable(args.path)[0];
        if ( value.empty() && variable_not_found )
            return VARIABLE_NOT_FOUND;
        fs::current_path(value);
    }
    catch ( std::exception & ex )
    {
        log("myshell: " + std::string {ex.what()} + "\n",
            OUT_TYPE::ERROR, true, args.error_output);
        return FAILURE;
    }

    if ( args.help )
    {
        log("mcd <path> [-h|--help] -- перейти до шляху <path>\n"
            "Після неї merrno повертає нуль, якщо вдалося перейти в нову директорію, не нуль -- якщо не вдалося або було передано невірні опції.\n",
            OUT_TYPE::OUT, true, args.output);
        return HELP_EXIT;
    }

    return SUCCESS;
}


EXIT_CODE mExit::run ( VecStr & parsed_line )
{
    Args args {};
    try
    {
        args = parse_arguments(parsed_line);
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }

    if ( args.help )
    {
        log("mexit [код завершення] [-h|--help]  – вийnb із myshell\n"
            "Якщо не передано код завершення -- вийде із кодом 0.\n",
            OUT_TYPE::OUT, true, args.output);
        return HELP_EXIT;
    }
    shell_exit_code = args.exit_code;
    return FORCE_EXIT;
}


EXIT_CODE mEcho::run ( VecStr & parsed_line )
{
    Args args {};
    try
    {
        args = parse_arguments(parsed_line);
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }
    if ( args.help )
    {
        log("mecho [text|$<var_name>] [text|$<var_name>]  [text|$<var_name>] ...\n"
                "Без аргументів не робить нічого. Аргументів може бути довільна кількість.\n"
                "Якщо аргумент починається не з $ -- просто виводить його на консоль.\n"
                "Якщо з $ -- шукає відповідну змінну та виводить її вміст. Якщо такої змінної не існує -- не виводить нічого.\n"
            "Після неї merrno повертає нуль.\n",
            OUT_TYPE::OUT, true, args.output);
        return HELP_EXIT;
    }

    if ( !args.words.empty())
    {
        VecStr values {};
        try
        {
            values = expand_arguments(args.words, args, true);
        }
        catch ( std::runtime_error & re )
        {
            log(re.what(), OUT_TYPE::ERROR, true, args.error_output);
            return VARIABLE_NOT_FOUND;
        }
        catch ( std::exception & ex )
        {
            log(ex.what(), OUT_TYPE::ERROR, true, args.error_output);
            return FAILURE;
        }

        for ( auto & value : values )
            log(value + " ", OUT_TYPE::OUT, false, args.output);
    }
    log("\n", OUT_TYPE::OUT, true, args.output);
    return SUCCESS;
}


EXIT_CODE mExport::run ( VecStr & parsed_line )
{
    Args args {};
    try
    {
        args = parse_arguments(parsed_line);
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }

    if ( args.help )
    {
        log("mexport <var_name>[=VAL]\n"
                "Додає глобальну змінну -- поміщається в блоку змінних середовища для дочірніх процесів.\n"
                "Якщо змінна не існувала, і не передано \"=VAL\", створюється як порожня.\n"
                "Якщо передано \"=VAL\", їй присвоюється відповідне значення. За потреби -- змінна створюється.\n"
            "Після неї merrno повертає нуль, якщо створити змінну вдалося, не нуль -- якщо ні.\n",
            OUT_TYPE::OUT, true, args.output);
        return HELP_EXIT;
    }

    auto delim_ind = args.var_name.find('=');
    if ( delim_ind == std::string::npos ) env_vars[args.var_name] = "";
    else env_vars[args.var_name.substr(0, delim_ind)] = args.var_name.substr(delim_ind + 1, std::string::npos);

    return SUCCESS;
}


EXIT_CODE mDot::run ( VecStr & parsed_line )
{
    Args args {};
    try
    {
        args = parse_arguments(parsed_line);
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }

    std::ifstream script_file;
    script_file.open(args.var_name, std::ios::in);

    if ( !script_file.is_open())
    {
        log("myshell: Cannot locate/open '" + args.var_name + "'.\n", OUT_TYPE::ERROR, true, args.output);
        return FAILURE;
    };

    std::string line;
    while ( getline(script_file, line))
    {
        if ( line[0] == '#' ) continue;
        auto parsed_line = parse_line(line);
        if ( parsed_line.size() > 1 )
            try_pipe(parsed_line, commands);
        else
        {
            auto command = parsed_line[0];
            if ( !commands.count(command[0]))
            {
                if ( try_to_execute(command) == SUCCESS )
                    continue;
                else if ( command.size() == 1 && try_add_var(command[0]) == SUCCESS )
                    continue;

                log("myshell: Command '" + command[0] + "' not found.\n", OUT_TYPE::ERROR, true);
                continue;
            }

            last_exit_code = commands[command[0]]->run(command);
        }
    }
    return SUCCESS;
}