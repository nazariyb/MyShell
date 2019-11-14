#include "commands.h"
#include "utils.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options.hpp>
//#include <fcntl.h>
#include <iostream>
#include <fstream>


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

Args Command::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr, false);
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen")
        //                (">", po::value<std::string>(), "Help screen")
        //                ("2>", po::value<std::string>(), "Help screen");


        po::positional_options_description pos_desc;
        //        pos_desc.add(">", -1);
        //        pos_desc.add("2>", -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        args.help = vm["help"].as<bool>();
    }
    catch ( const po::error & ex )
    {
        log(std::string {ex.what()} + '\n',
            OUT_TYPE::ERROR, true, "out.txt");
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mCd::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr, false);
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen")
                ("path", po::value<std::string>(), "Help screen")
        //                (">", po::value<std::string>(), "Help screen")
        //                ("2>", po::value<std::string>(), "Help screen");

        po::positional_options_description pos_desc;
        pos_desc.add("path", -1);
        //        pos_desc.add(">", -1);
        //        pos_desc.add("2>", -1);



        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
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
        log(std::string {ex.what()} + '\n',
            OUT_TYPE::ERROR, true, "out.txt");
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mExit::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr, false);
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", po::bool_switch(), "Help screen")
                ("exit_code", po::value<int>(), "Help screen")
        //                (">", po::value<std::string>(), "Help screen")
        //                ("2>", po::value<std::string>(), "Help screen");


        po::positional_options_description pos_desc;
        pos_desc.add("exit_code", -1);
        //        pos_desc.add(">", -1);
        //        pos_desc.add("2>", -1);

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
        log(std::string {ex.what()} + '\n',
            OUT_TYPE::ERROR, true, "out.txt");
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mEcho::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr, false);

    try
    {
        od desc {"Options"};
        desc.add_options()
                ("words", po::value<std::vector<std::string>>()->
                        multitoken()->zero_tokens()->composing(), "Help screen");
        //                (">", po::value<std::string>(), "Help screen")
        //                ("2>", po::value<std::string>(), "Help screen");


        po::positional_options_description pos_desc;
        pos_desc.add("words", -1);
        //        pos_desc.add(">", -1);
        //        pos_desc.add("2>", -1);


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
        log(std::string {ex.what()} + '\n',
            OUT_TYPE::ERROR, true, "out.txt");
        delete[]( argv );
        throw ex;
    }
    delete[]( argv );
    return args;
}


Args mExport::parse_arguments ( const VecStr & vecStr )
{
    Args args {};
    auto argv = vecstr2char(vecStr, false);

    try
    {
        od desc {"Options"};
        desc.add_options()
                ("var_name", po::value<std::string>(), "Help screen")
        //                (">", po::value<std::string>(), "Help screen")
        //                ("2>", po::value<std::string>(), "Help screen");


        po::positional_options_description pos_desc;
        pos_desc.add("var_name", -1);
        //        pos_desc.add(">", -1);
        //        pos_desc.add("2>", -1);

        po::command_line_parser parser {static_cast<int>(vecStr.size()), argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .run();


        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        if ( vm.count("var_name"))
            args.var_name = vm["var_name"].as<std::string>();

    }
    catch ( const po::error & ex )
    {
        log(std::string {ex.what()} + '\n',
            OUT_TYPE::ERROR, true, "out.txt");
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
            log("merrno [-h|--help] – вивести код завершення останньої програми чи команди\n"
                    "Повертає нуль, якщо ще жодна програма не виконувалася.\n"
                "Після неї самої merrno повертає нуль, крім випадку, коли було передано невірні опції.\n",
                OUT_TYPE::OUT, true, "out.txt");
            return HELP_EXIT;
        }
        log(std::to_string(last_exit_code) + "\n",
            OUT_TYPE::OUT, true, "out.txt");
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
            log("mpwd [-h|--help] – вивести поточний шлях\n"
                "Після неї merrno повертає нуль, крім випадку, коли було передано невірні опції.",
                OUT_TYPE::OUT, true, "out.txt");
            return HELP_EXIT;
        }
        log(fs::current_path().string(), OUT_TYPE::OUT, true, "out.txt");
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
            log("mcd <path> [-h|--help] -- перейти до шляху <path>\n"
                "Після неї merrno повертає нуль, якщо вдалося перейти в нову директорію, не нуль -- якщо не вдалося або було передано невірні опції.\n",
                OUT_TYPE::OUT, true, "out.txt");
            return HELP_EXIT;
        }
        auto value = get_variable(args.path)[0];
        if ( value.empty() && variable_not_found )
            return VARIABLE_NOT_FOUND;
        fs::current_path(value);
    }
    catch ( std::exception & ex )
    {
        log("myshell: " + std::string {ex.what()} + "\n",
            OUT_TYPE::ERROR, true, "out.txt");
        return FAILURE;
    }
    return SUCCESS;
}


EXIT_CODE mExit::run ( const VecStr & parsed_line )
{
    auto args = parse_arguments(parsed_line);
    if ( args.help )
    {
        log("mexit [код завершення] [-h|--help]  – вийnb із myshell\n"
            "Якщо не передано код завершення -- вийде із кодом 0.\n",
            OUT_TYPE::OUT, true, "out.txt");
        return HELP_EXIT;
    }
    shell_exit_code = args.exit_code;
    return FORCE_EXIT;
    //    throw Exit(args.exit_code);
}


EXIT_CODE mEcho::run ( const VecStr & parsed_line )
{
    auto args = parse_arguments(parsed_line);

    if ( args.help )
    {
        log("mecho [text|$<var_name>] [text|$<var_name>]  [text|$<var_name>] ...\n"
                "Без аргументів не робить нічого. Аргументів може бути довільна кількість.\n"
                "Якщо аргумент починається не з $ -- просто виводить його на консоль.\n"
                "Якщо з $ -- шукає відповідну змінну та виводить її вміст. Якщо такої змінної не існує -- не виводить нічого.\n"
            "Після неї merrno повертає нуль.\n",
            OUT_TYPE::OUT, true);
        return HELP_EXIT;
    }

    if ( !args.words.empty())
    {
        VecStr values {};
        try
        {
            values = expand_arguments(args.words);
        }
        catch ( std::runtime_error & re )
        {
            // std::cerr << re.what() << endl;
            return VARIABLE_NOT_FOUND;
        }
        catch ( std::exception & ex )
        {
            log(ex.what(), OUT_TYPE::ERROR, true);
            return FAILURE;
        }

        for ( auto & value : values )
            log(value + " ", OUT_TYPE::OUT, false, "out.txt");
    }
    log("\n", OUT_TYPE::OUT, true, "out.txt");
    return SUCCESS;
}


EXIT_CODE mExport::run ( const VecStr & parsed_line )
{
    auto args = parse_arguments(parsed_line);

    if ( args.help )
    {
        log("mexport <var_name>[=VAL]\n"
                "Додає глобальну змінну -- поміщається в блоку змінних середовища для дочірніх процесів.\n"
                "Якщо змінна не існувала, і не передано \"=VAL\", створюється як порожня.\n"
                "Якщо передано \"=VAL\", їй присвоюється відповідне значення. За потреби -- змінна створюється.\n"
            "Після неї merrno повертає нуль, якщо створити змінну вдалося, не нуль -- якщо ні.\n",
            OUT_TYPE::OUT, true);
        return HELP_EXIT;
    }

    auto delim_ind = args.var_name.find('=');
    if ( delim_ind == std::string::npos ) env_vars[args.var_name] = "";
    else env_vars[args.var_name.substr(0, delim_ind)] = args.var_name.substr(delim_ind + 1, std::string::npos);

    return SUCCESS;
}
