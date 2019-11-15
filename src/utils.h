#ifndef MYSHELL_UTILS_H
#define MYSHELL_UTILS_H


#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/regex.hpp>


namespace fs = boost::filesystem;
namespace po = boost::program_options;
using od = po::options_description;

using VecStr = std::vector<std::string>;
using MapStrStr = std::map<std::string, std::string>;

using std::cout;
using std::endl;

enum EXIT_CODE
    {
    SUCCESS = 0, HELP_EXIT = 0, FAILURE, FORCE_EXIT, COMMAND_EXECUTION_ERROR, NOT_VALID_VARIABLE,
    VARIABLE_NOT_FOUND
    };

extern EXIT_CODE last_exit_code;
extern int shell_exit_code;
extern VecStr PATH;
extern MapStrStr vars;
extern MapStrStr env_vars;
extern bool variable_not_found;

using Args = struct Args
    {
    bool help {false};
    std::string path {};
    int exit_code {0};
    VecStr words {};
    std::string var_name {};
    std::string input {};
    std::string output {};
    std::string error_output {};
    };

char ** vecstr2char ( VecStr vecStr, bool add_null );


std::vector<std::string> parse_line ( const std::string & commandline );

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

//class Exit : public std::runtime_error
//    {
//public:
//    explicit Exit ( int n ) : std::runtime_error(std::to_string(n))
//    { }
//
//
//    int what ()
//    { return std::stoi(std::runtime_error::what()); }
//    };


int try_to_execute ( const std::vector<std::string> & command_opts );
//void fork_exec(const std::string & exec_name, const VecStr & arguments);


std::vector<std::string> open_wildcard ( const std::string & path );

std::vector<std::string> map2vecstr ( const MapStrStr & vars_map );

int try_add_var ( const std::string & pair );

VecStr get_variable ( const std::string & word_ );

VecStr expand_arguments ( const VecStr & arguments );

#endif //MYSHELL_UTILS_H
