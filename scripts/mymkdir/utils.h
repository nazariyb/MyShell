#ifndef MYMKDIR_UTILS_H
#define MYMKDIR_UTILS_H

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include <string>

namespace fs = boost::filesystem;
namespace po = boost::program_options;
using od = po::options_description;


struct Args
{
    std::string path;
    bool help;
    bool p;
};

enum ReturnStatus
{
    SUCCESS,  ERROR_EXIT, PARSE_ARG_ERR
};

Args parse_arguments ( int argc, char * argv[] );
std::vector<std::string> split_path(std::string path, std::string delim);



#endif //MYMKDIR_UTILS_H
