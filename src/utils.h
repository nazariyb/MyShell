#ifndef MYSHELL_UTILS_H
#define MYSHELL_UTILS_H


#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>


namespace fs = boost::filesystem;
namespace po = boost::program_options;
using od = po::options_description;

using VecStr = std::vector<std::string>;

using std::cout;
using std::endl;

enum EXIT_CODE
    {
    SUCCESS, FAILURE, FORCE_EXIT, PRINT_LAST_CODE
    };

extern EXIT_CODE last_exit_code;// {static_cast<EXIT_CODE>(0)};


using Args = struct Args
    {
    bool help {false};
    std::string path {};
    int exit_code {0};
    VecStr words {};
    std::string var_name {};
    };

char ** vecstr2char ( VecStr vecStr );


void parse_arguments ( const VecStr & vecStr, Args & args );


class Exit : public std::runtime_error
    {
public:
    explicit Exit ( int n ) : std::runtime_error(std::to_string(n))
    { }


    int what ()
    { return std::stoi(std::runtime_error::what()); }
    };


#endif //MYSHELL_UTILS_H
