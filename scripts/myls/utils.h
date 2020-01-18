#ifndef MYSHELL_UTILS_H
#define MYSHELL_UTILS_H

#include <boost/filesystem.hpp>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <boost/program_options.hpp>


namespace fs = boost::filesystem;
namespace po = boost::program_options;
using od = po::options_description;
using VecStr = std::vector<std::string>;

using std::cout;
using std::cerr;
using std::endl;

enum EXIT_CODE
    {
    SUCCESS = 0, HELP_EXIT = 0, PRINT_TREE = 0, FAILURE,
    PARSE_ARGUMENTS_ERROR, GET_FILES_LIST_ERROR, SORT_ERROR, PRINT_FILES_ERROR
    };

extern int exit_code;

enum FILE_TYPE
    {
    EXECUTABLE, REGULAR, SYMLINK, CHANNEL, SOCKET, OTHER, DIRECTORY
    };

static std::map<FILE_TYPE, std::string> file_type_to_symbol =
        {
                {EXECUTABLE, "*"},
                {REGULAR,    ""},
                {SYMLINK,    "@"},
                {CHANNEL,    "|"},
                {SOCKET,     "="},
                {OTHER,      "?"},
                {DIRECTORY,  "/"}
        };


struct FileType
    {
    std::string type_symbol;
    FILE_TYPE type_name;


    explicit FileType ( FILE_TYPE type )
            : type_symbol(file_type_to_symbol[type]), type_name {type}
    { }
    };


inline std::ostream & operator<< ( std::ostream & os, const FileType & fileType )
{
    os << fileType.type_symbol;
    return os;
}


struct File
    {
    std::string name;
    boost::uintmax_t size;
    std::string date;
    std::string time;
    std::time_t full_time;
    FileType type;


    File ( std::string name_,
           const boost::uintmax_t & size_,
           std::string date_,
           std::string time_,
           std::time_t full_time_,
           FileType type_ ) :
            name {std::move(name_)},
            size {size_},
            date {std::move(date_)},
            time {std::move(time_)},
            full_time {full_time_},
            type {std::move(type_)}
    { }
    };


using VecFile = std::vector<File>;
using MapVecFile = std::map<std::string, VecFile>;

struct Args
    {
    VecStr path;
    bool detail_info;
    bool help;
    std::string sort_type;
    bool reverse;
    bool show_special_types;
    bool recursive;
    bool tree;
    };

Args parse_arguments ( int argc, char * argv[] );

MapVecFile get_files ( VecStr & path_, bool recursive );

void sort_files ( MapVecFile & files, const Args & args );

void print_files ( MapVecFile & files, const Args & args );

namespace Color {
    enum Code
        {
        FG_RED = 31,
        FG_GREEN = 32,
        FG_BLUE = 34,
        FG_DEFAULT = 39,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_BLUE = 44,
        BG_DEFAULT = 49
        };


    class Modifier
        {
        Code code;
    public:
        explicit Modifier ( Code pCode ) : code(pCode)
        { }


        friend std::ostream &
        operator<< ( std::ostream & os, const Modifier & mod )
        {
            return os << "\033[" << mod.code << "m";
        }
        };
}

#endif //MYSHELL_UTILS_H
