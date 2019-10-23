#include "commands.h"
#include "utils.h"

#include <cstdlib>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options.hpp>


Args Command::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr);
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen");

        po::positional_options_description pos_desc;
        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                        //                                   .allow_unregistered()
                .run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        args.help = vm["help"].as<bool>();
    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mCd::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr);
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen")
                ("path", po::value<std::string>(), "Help screen");

        po::positional_options_description pos_desc;
        pos_desc.add("path", -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                        //                                   .allow_unregistered()
                .run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        args.help = vm["help"].as<bool>();
        if ( !vm.count("path")) throw std::invalid_argument("path is not specified!");
        args.path = vm["path"].as<std::string>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mExit::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr);
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen")
                ("exit_code", po::value<int>(), "Help screen");

        po::positional_options_description pos_desc;
        pos_desc.add("exit_code", -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .allow_unregistered()
                .run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        args.help = vm["help"].as<bool>();

        if ( vm.count("exit_code"))
            args.exit_code = vm["exit_code"].as<int>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mEcho::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr);

    try
    {
        od desc {"Options"};
        desc.add_options()
                ("words", po::value<std::vector<std::string>>()->
                        multitoken()->zero_tokens()->composing(), "Help screen");

        po::positional_options_description pos_desc;
        pos_desc.add("words", -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .allow_unregistered()
                .run();


        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        if ( vm.count("words"))
            args.words = vm["words"].as<VecStr>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mExport::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr);

    try
    {
        od desc {"Options"};
        desc.add_options()
                ("words", po::value<std::vector<std::string>>()->
                        multitoken()->zero_tokens()->composing(), "Help screen");

        po::positional_options_description pos_desc;
        pos_desc.add("words", -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .allow_unregistered()
                .run();


        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        if ( vm.count("words"))
            args.words = vm["words"].as<VecStr>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


EXIT_CODE mErrno::run ( const VecStr & parsed_line )
{
    try
    {
        auto args = parse_arguments(parsed_line);
        if ( args.help )
        {
            cout << "errno help" << endl;
            return HELP_EXIT;
        }
        cout << last_exit_code << endl;
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }

    return SUCCESS;
}


EXIT_CODE mPwd::run ( const VecStr & parsed_line )
{
    try
    {
        auto args = parse_arguments(parsed_line);
        if ( args.help )
        {
            cout << "pwd help" << endl;
            return HELP_EXIT;
        }
        cout << fs::current_path() << endl;
    }
    catch ( std::exception & ex )
    {
        return FAILURE;
    }
    return SUCCESS;
}


EXIT_CODE mCd::run ( const VecStr & parsed_line )
{
    try
    {
        auto args = parse_arguments(parsed_line);
        if ( args.help )
        {
            cout << "cd help" << endl;
            return HELP_EXIT;
        }
        fs::current_path(args.path);
    }
    catch ( std::exception & ex )
    {
        std::cerr << "myshell: " << ex.what() << endl;
        return FAILURE;
    }
    return SUCCESS;
}


EXIT_CODE mExit::run ( const VecStr & parsed_line )
{
    auto args = parse_arguments(parsed_line);
    if ( args.help )
    {
        cout << "exit help" << endl;
        return HELP_EXIT;
    }
    shell_exit_code = args.exit_code;
    return FORCE_EXIT;
    //    throw Exit(args.exit_code);
}


EXIT_CODE mEcho::run ( const VecStr & parsed_line )
{
    auto args = parse_arguments(parsed_line);

    if ( !args.words.empty())
        for ( auto & word: args.words )
            cout << word << " ";

    cout << endl;
    return SUCCESS;
}


EXIT_CODE mExport::run ( const VecStr & parsed_line )
{
    //    auto args = parse_arguments(parsed_line);
    return SUCCESS;
}
