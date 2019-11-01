#include "boost/program_options.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>


namespace po = boost::program_options;
using od = po::options_description;

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

enum ReturnStatus
    {
    SUCCESS, SUCCESS_EXIT, ON_ERROR_EXIT, PARSE_ARG_ERR, OPEN_FILE_ERR, READ_FILE_ERR
    };


int my_print ( int fd, const char * buffer, ssize_t size )
{
    ssize_t written_bytes = 0;
    while ( written_bytes < size )
    {
        ssize_t written_now = write(fd, buffer + written_bytes, size - written_bytes);
        if ( written_now == -1 )
        {
            if ( errno == EINTR )
                continue;
            else
                return ON_ERROR_EXIT;
        }
        else
            written_bytes += written_now;
    }
    return SUCCESS;
}


int parse_args ( const int argc, const char * argv[], std::vector<std::string> & files, bool & print_hex4invisible )
{
    try
    {
        od desc {"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                (",A", po::bool_switch(), "Change non-printable symbols to hex")
                ("files", po::value<std::vector<std::string>>()->
                         multitoken()->zero_tokens()->composing(),
                 "All arguments without flags belong to 'files'. It is not needed to specify it");

        po::positional_options_description pos_desc;
        pos_desc.add("files", -1);

        po::command_line_parser parser {argc, argv};
        parser.options(desc).positional(pos_desc).allow_unregistered();
        po::parsed_options parsed_options = parser.run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        if ( vm.count("help"))
        {
            std::cout << desc << '\n';
            return SUCCESS_EXIT;
        }

        print_hex4invisible = vm["-A"].as<bool>();

        if ( vm.count("files"))
            files = vm["files"].as<std::vector<std::string>>();
    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        return ON_ERROR_EXIT;
    }

    return SUCCESS;
}


int open_files ( const std::vector<std::string> & files, std::vector<int> & fd_vector )
{
    int fd {0};

    for ( auto & f: files )
    {

        fd = open(f.c_str(), O_RDONLY);

        if ( fd == -1 )
        {
            std::string str_err = strerror(errno);
            str_err += ": " + f + "\n";
            my_print(STDERR_FILENO, str_err.c_str(), str_err.size());

            for ( auto & _fd: fd_vector )
            {
                if ( close(_fd) == -1 )
                {
                    std::string str_err = strerror(errno);
                    str_err += ": " + std::to_string(_fd) + "\n";
                    my_print(STDERR_FILENO, str_err.c_str(), str_err.size());
                }
            }
            return ON_ERROR_EXIT;
        }
        fd_vector.push_back(fd);
    }
    return SUCCESS;
}


size_t translate_hex ( char * file_buffer, std::string * out_buffer, int num_read, bool print_hex4invisible )
{
    if ( !print_hex4invisible )
    {
        *out_buffer = file_buffer;
        return num_read;
    }

    std::stringstream hex_char;
    char current_char;

    for ( int i = 0; i < num_read; ++i )
    {
        current_char = file_buffer[i];

        if ( !( isprint(current_char) || isspace(current_char)))
            hex_char << std::hex << current_char;
        else
            hex_char << current_char;

        *out_buffer += hex_char.str();
        hex_char.str("");
    }

    return ( *out_buffer ).size();
}


int read_and_print ( std::vector<int> & fd_vector, const bool print_hex4invisible )
{
    char file_buffer[BUF_SIZE];
    std::string out_buffer {};
    size_t out_buf_size;
    ssize_t num_read {0};

    for ( auto & fd: fd_vector )
    {
        while (( num_read = read(fd, &file_buffer, BUF_SIZE)))
        {
            if ( num_read == -1 )
            {
                if ( errno == EINTR )
                    continue;
                else
                {
                    std::string str_err = strerror(errno);
                    str_err += ": " + std::to_string(fd) + "\n";
                    my_print(STDERR_FILENO, str_err.c_str(), str_err.size());
                    return ON_ERROR_EXIT;
                }
            }
            else
            {
                out_buf_size = translate_hex(file_buffer, &out_buffer, num_read, print_hex4invisible);
                my_print(STDOUT_FILENO, out_buffer.c_str(), out_buf_size);
            }
            out_buffer.clear();
        }
    }
    return SUCCESS;
}


void close_files ( const std::vector<int> & fd_vector )
{
    for ( auto & fd: fd_vector )
    {
        if ( close(fd) == -1 )
        {
            std::string str_err = strerror(errno);
            str_err += ": " + std::to_string(fd) + "\n";
            my_print(STDERR_FILENO, str_err.c_str(), str_err.size());
        }
    }
}


int main ( int argc, const char * argv[] )
{
    std::vector<std::string> files;
    bool print_hex4invisible {false};
    auto parse_status = parse_args(argc, argv, files, print_hex4invisible);
    if ( parse_status != SUCCESS )
    {
        if ( parse_status == SUCCESS_EXIT )
            _exit(SUCCESS);
        else if ( parse_status == PARSE_ARG_ERR )
            _exit(PARSE_ARG_ERR);
    }

    std::vector<int> fd_vector;
    if ( open_files(files, fd_vector) != SUCCESS )
        _exit(OPEN_FILE_ERR);

    if ( read_and_print(fd_vector, print_hex4invisible) != SUCCESS )
        _exit(READ_FILE_ERR);

    close_files(fd_vector);

    _exit(SUCCESS);
}