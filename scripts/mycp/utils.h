#ifndef MYCP_UTILS_H
#define MYCP_UTILS_H

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <experimental/filesystem>
#include <boost/algorithm/string.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>

namespace fs = boost::filesystem;
namespace efs = std::experimental::filesystem;
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
    SUCCESS,  ERROR_EXIT, PARSE_ARG_ERR, FILE_TYPE_ERR
};

Args parse_arguments ( int argc, char * argv[] );

void recursive_copy(const fs::path &src, const fs::path &dst, bool sm);

bool my_copy_file(const fs::path &src, const fs::path &dst, bool silent_mode);


#endif //MYCP_UTILS_H
