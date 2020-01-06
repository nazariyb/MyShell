#ifndef MYRM_UTILS_H
#define MYRM_UTILS_H

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace fs = boost::filesystem;
namespace po = boost::program_options;
using od = po::options_description;
using VecStr = std::vector<std::string>;


struct Args
{
    VecStr pathes;
    bool help;
    bool silent_mode;
    bool recursive;
};

enum ReturnStatus
{
    SUCCESS,  ERROR_EXIT, PARSE_ARG_ERR
};

Args parse_arguments ( int argc, char * argv[] );


#endif //MYRM_UTILS_H
