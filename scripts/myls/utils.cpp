#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <iomanip>

#include <boost/algorithm/string.hpp>

#include "utils.h"
#include "table_printer/table_printer.h"


int exit_code {SUCCESS};


Args parse_arguments ( int argc, char * argv[] )
{
    Args args;
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("path", po::value<VecStr>()->multitoken()->zero_tokens()->composing())
                (",-", po::value<VecStr>()->multitoken()->zero_tokens()->composing())
                (",l", po::bool_switch())
                ("help,h", po::bool_switch())
                ("sort", po::value<std::string>())
                (",r", po::bool_switch())
                (",F", po::bool_switch())
                (",R", po::bool_switch())
                ("tree", po::bool_switch());
        po::positional_options_description pos_desc;
        pos_desc.add("path", -1);
        pos_desc.add("--", -1);
        po::command_line_parser parser {argc, argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .allow_unregistered()
                .run();


        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        if ( vm.count("path"))
            args.path = vm["path"].as<VecStr>();

        if ( vm.count("--"))
            args.path = vm["--"].as<VecStr>();


        args.detail_info = vm["-l"].as<bool>();

        args.help = vm["help"].as<bool>();

        args.sort_type = (( vm.count("sort"))) ? vm["sort"].as<std::string>() : "N";
        if ( args.sort_type.find("D") != std::string::npos )
        {
            boost::erase_all(args.sort_type, "D");
            args.sort_type += "D";
        }

        args.reverse = vm["-r"].as<bool>();

        args.show_special_types = vm["-F"].as<bool>();

        args.recursive = vm["-R"].as<bool>();

        args.tree = vm["tree"].as<bool>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        throw ex;
    }
    return args;
}


std::string date2str ( const std::time_t & t )
{
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%e");
    return ss.str();
}


std::string time2str ( const std::time_t & t )
{
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&t), "%H:%M");
    return ss.str();
}


FILE_TYPE file_type ( const fs::path & status )
{
    auto type = fs::status(status).type();

    if ( type == fs::file_type::directory_file )
        return DIRECTORY;
    if ( fs::symlink_status(status).type() == fs::symlink_file )
        return SYMLINK;
    if ( type == fs::file_type::fifo_file )
        return CHANNEL;
    if ( type == fs::file_type::socket_file )
        return SOCKET;
    if ( fs::status(status).permissions() & S_IEXEC )
        return EXECUTABLE;
    if ( type == fs::file_type::regular_file )
        return REGULAR;
    return OTHER;
}


boost::uintmax_t directory_size ( const fs::path & dir_path )
{
#ifdef __linux__
    return 4096;
#else
    return 0;
#endif

    //  Logic does not work here :(
    //    boost::uintmax_t dir_size {0};
    //
    //    for ( fs::recursive_directory_iterator it(dir_path);
    //          it != fs::recursive_directory_iterator();
    //          ++it )
    //        if ( !fs::is_directory(*it))
    //            dir_size += fs::file_size(*it);
    //
    //    return dir_size;
}


void add_file ( VecFile & files, const fs::path & full_path )
{
    auto current_filename = full_path.filename();
    auto t = fs::last_write_time(full_path);

    files.emplace_back(
            current_filename.string(),
            ( fs::is_directory(full_path)) ? directory_size(full_path) : fs::file_size(full_path),
            date2str(t),
            time2str(t),
            t,
            FileType {file_type(full_path)}
    );
}


MapVecFile get_files ( VecStr & paths, bool recursive )
{
    MapVecFile files;
    if ( paths.empty())
        paths.emplace_back(".");

    for ( auto & path: paths )
    {
        try
        {
            if ( fs::is_directory(path))
            {
                if ( recursive )
                {
                    for ( fs::recursive_directory_iterator end_itr, i(path); i != end_itr; ++i )
                    {
                        if ( fs::is_directory(i->path()) && !files.count(i->path().string()))
                            files[i->path().string()] = VecFile {};
                        add_file(files[i->path().parent_path().string()], i->path());
                    }
                }
                else
                {
                    for ( fs::directory_iterator end_itr, i(path); i != end_itr; ++i )
                    {
                        if ( !files.count(path))
                            files[path] = VecFile {};
                        add_file(files[path], i->path());
                    }
                }
            }
            else
            {
                if ( !files.count("."))
                    files["."] = VecFile {};
                add_file(files["."], fs::path {path});
            }
        }
        catch ( std::exception & ex )
        {
            cerr << "myls: " << ex.what() << endl;
            exit_code = GET_FILES_LIST_ERROR;
        }
    }

    return files;
}


bool is_special_file ( const File & file )
{
    return (( file.type.type_name != REGULAR )
            && ( file.type.type_name != DIRECTORY )
            && ( file.type.type_name != EXECUTABLE ));
}


void sort_files ( MapVecFile & files, const Args & args )
{
    std::map<char, std::function<bool ( const File & a, const File & b )>> sorters = {
            {'U', [args] ( const File & a, const File & b ) { return true; }},

            {'S', [args] ( const File & a, const File & b ) {
                return (
                        ( a.size != b.size )
                        ? a.size < b.size
                        : a.name.compare(b.name) < 0
                );
            }},

            {'t', [args] ( const File & a, const File & b ) {
                return (
                        ( a.full_time != b.full_time )
                        ? a.full_time > b.full_time
                        : a.name.compare(b.name) < 0
                );
            }},

            {'X', [args] ( const File & a, const File & b ) {
                return (
                        (( a.name.find_last_of('.') != std::string::npos )
                         ? a.name.substr(a.name.find_last_of('.'), std::string::npos)
                         : ""
                        ).compare(
                                ( b.name.find_last_of('.') != std::string::npos )
                                ? b.name.substr(b.name.find_last_of('.'), std::string::npos)
                                : ""
                        ) < 0 );
            }},

            {'N', [args] ( const File & a, const File & b ) { return ( a.name.compare(b.name) < 0 ); }},

            {'D', [args] ( const File & a, const File & b ) {
                return (
                        ( a.type.type_name == DIRECTORY && b.type.type_name == DIRECTORY )
                        ? a.name.compare(b.name) < 0
                        : a.type.type_name == DIRECTORY && b.type.type_name != DIRECTORY
                );
            }},

            {'s', [args] ( const File & a, const File & b ) {
                return (
                        ( is_special_file(a) && is_special_file(b) )
                        ? a.name.compare(b.name) < 0
                        : is_special_file(a) && !is_special_file(b)
                );
            }}
    };

    for ( auto & sort_type: args.sort_type )
        if ( !sorters.count(sort_type))
            throw std::runtime_error(
                    "Unknown argument '" + std::string(1, sort_type) + "'. Use --help|-h for more info."
            );

    if ( args.sort_type != "U" )
        for ( auto & vecFile : files )
            for ( auto & sort_type: args.sort_type )
                std::sort(vecFile.second.begin(), vecFile.second.end(), sorters[sort_type]);

    if ( args.reverse )
        for ( auto & vecFile : files )
            std::reverse(vecFile.second.begin(), vecFile.second.end());
}


inline void print_with_details ( const File & file, bprinter::TablePrinter & tp )
{
    tp << file.name << file.size << file.date << file.time;
}


inline void just_print ( const File & file, bprinter::TablePrinter & tp )
{
    cout << file.name << " ";
}


void print_file ( File & file, const Args & args, bprinter::TablePrinter & tp )
{
    if ( file.type.type_name == DIRECTORY || args.show_special_types )
        file.name = file.type.type_symbol + file.name;
    ( args.detail_info )
    ? print_with_details(file, tp)
    : just_print(file, tp);
}


void init_table ( bprinter::TablePrinter & tp )
{
    tp.AddColumn("Name", 25);
    tp.AddColumn("Size", 15);
    tp.AddColumn("Date", 16);
    tp.AddColumn("Time", 10);
}


void print_files ( MapVecFile & files, const Args & args )
{
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);
    if ( files.size() == 1 )
        for ( auto & files_ : files )
        {
            bprinter::TablePrinter tp(&std::cout);
            if ( args.detail_info )
            {
                init_table(tp);
                tp.PrintHeader();
            }

            for ( auto & file: files_.second )
                print_file(file, args, tp);

            if ( args.detail_info )
                tp.PrintFooter();
            else
                cout << endl;
        }
    else
        for ( auto & p: files )
        {
            bprinter::TablePrinter tp(&std::cout);

            if ( p.first != "." )
                cout << endl << green << p.first << ": " << def << endl;

            if ( args.detail_info )
            {
                init_table(tp);
                tp.PrintHeader();
            }

            for ( auto & file : p.second )
                print_file(file, args, tp);

            if ( args.detail_info )
                tp.PrintFooter();
            else
                cout << endl;

        }
}