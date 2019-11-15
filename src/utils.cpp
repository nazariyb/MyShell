#include <boost/program_options/positional_options.hpp>
#include <boost/algorithm/string.hpp>

#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <map>
#include <fnmatch.h>
#include <fcntl.h>

#include "utils.h"
#include "commands.h"


char ** vecstr2char ( VecStr vecStr, bool add_null )
{
    auto vec_size = vecStr.size();
    auto argv = new char * [vec_size + add_null];

    for ( size_t i = 0; i < vec_size; ++i )
        argv[i] = (char *) (vecStr[i].c_str());

    if ( add_null )
        argv[vec_size] = nullptr;

    return argv;
}


std::vector<std::string> parse_line ( const std::string & commandline )
{
    bool quotes_are_opened{false};
    std::vector<std::string> args{};
    std::string buff{};

    for ( auto & c: commandline )
    {
        if ( c == ' ' && buff.empty() && args.empty()) continue;
        if (c == '"') quotes_are_opened ^= static_cast<unsigned>(1);

        if ((c == ' ') && (!quotes_are_opened)) {
            args.push_back(buff);
            buff.clear();
            continue;
        }
        buff += c;
    }
    args.push_back(buff);
    return args;
}


VecStr expand_arguments ( VecStr & arguments, Args & args )
{
    VecStr expanded_args {};

    Command::parse_redirections(arguments, args);

    for ( auto & arg: arguments )
    {
        auto vars = get_variable(arg);
        for ( auto & var: vars )
        {
            if ( var.empty() && variable_not_found )
                throw std::runtime_error("Variable " + arg + " not found");
            expanded_args.push_back(var);
        }
    }

    return expanded_args;
}


int redirect ( const Args & args )
{
    int fd;
    if ( !args.input.empty())
    {
        fd = open(args.input.c_str(), O_RDONLY);
        if ( !( fcntl(fd, F_GETFD) != -1 || errno != EBADF )) return -1;
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if ( !args.output.empty())
    {
        // read & write for file's owner, read for group & others
        fd = creat(args.output.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if ( !( fcntl(fd, F_GETFD) != -1 || errno != EBADF )) return -1;
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    if ( !args.error_output.empty())
    {
        // read & write for file's owner, read for group & others
        if ( !( args.error_output == args.output ))
        {
            fd = creat(args.error_output.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if ( !( fcntl(fd, F_GETFD) != -1 || errno != EBADF )) return -1;
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        else
            dup2(STDOUT_FILENO, STDERR_FILENO);
    }
    return SUCCESS;
}


void fork_exec ( const std::string & exec_name, VecStr & arguments_ )
{

    //    pid_t parent = getpid();
    pid_t pid = fork();


    if (pid == -1) {
        throw std::runtime_error("Failed to fork");
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        last_exit_code = static_cast<EXIT_CODE>(status);
    } else {

        VecStr arguments {};
        Args args_struct {};
        try
        {
            arguments = expand_arguments(arguments_, args_struct);
        }
        catch ( std::exception & ex )
        {
            throw ex;
        }

        if ( redirect(args_struct) != SUCCESS )
            throw std::runtime_error("unable to locate/create file for input/output");

        arguments[0] = fs::path {exec_name}.filename().string();
        auto args = vecstr2char(arguments, true);
        auto env = vecstr2char(map2vecstr(env_vars), true);

        execve(exec_name.c_str(), args, env);

        delete[] args;
        delete[] env;
    }
}


int try_to_execute ( VecStr & command_opts )
{
    auto command_name = command_opts[0];

    try
    {
        for ( auto & loc: PATH )
        {
            if ( boost::filesystem::exists(loc + command_name) & ( !access(( loc + command_name ).c_str(), X_OK)))
            {
                fork_exec(loc + command_name, command_opts);
                return SUCCESS;
            }
        }
        if ( !access(command_name.c_str(), X_OK))
        {
            fork_exec(command_name, command_opts);
            return SUCCESS;
        }
    }
    catch ( std::exception & ex )
    {
        std::cerr << "myshell: " << ex.what() << endl;
    }
    return COMMAND_EXECUTION_ERROR;
}


VecStr open_wildcard ( const std::string & path_ )
{
    auto path = fs::path {path_};
    auto parent_path = path.parent_path().string();
    auto dir = ( parent_path.empty()) ? "." : parent_path;

    auto filename = path.filename().string();
    auto pattern = filename.c_str();

    VecStr matching_files;
    try
    {
        for ( fs::directory_iterator end_itr, i(dir); i != end_itr; ++i )
        {

            auto current_filename = i->path().filename().string();
            if ( !boost::filesystem::is_regular_file(i->status())) continue;

            //      fnmatch returns 0 if string matches the pattern
            if ( fnmatch(pattern, current_filename.c_str(), FNM_FILE_NAME)) continue;

            matching_files.push_back(i->path().filename().string());
        }
    }
    catch ( std::exception & ex )
    {
        //        std::cerr << "myshell: " << ex.what() << endl;
    }

    return ( matching_files.empty()) ? VecStr {path_} : matching_files;
}


std::vector<std::string> map2vecstr ( const MapStrStr & vars_map )
{

    std::vector<std::string> vars_vec;

    for ( auto const&[key, val] : vars_map )
        vars_vec.emplace_back(key + "=" + val);

    return vars_vec;
}


int try_add_var ( const std::string & pair )
{
    auto delim_ind = pair.find('=');

    if ( delim_ind == std::string::npos ) return NOT_VALID_VARIABLE;
    else vars[pair.substr(0, delim_ind)] = pair.substr(delim_ind + 1, std::string::npos);

    return SUCCESS;
}


VecStr get_variable ( const std::string & word_ )
{
    if ( boost::starts_with(word_, "$"))
    {
        auto word = word_.substr(1, std::string::npos);

        if ( vars.count(word))
        {
            variable_not_found = false;
            return {vars[word]};
        }
        else if ( env_vars.count(word))
        {
            variable_not_found = false;
            return {env_vars[word]};
        }
        else
        {
            std::cerr << "\nmyshell: Variable '" << word << "' not found.\n" << endl;
            variable_not_found = true;
            return {std::string {""}};
        }
    }
    auto list = open_wildcard(word_);
    return ( list.empty()) ? VecStr {word_} : list;
}


void redirect ( bool in, bool out, const std::string & input, const std::string & output )
{
    if ( in )
    { //if '<' char was found in string inputted by user
        auto fd = open(input.c_str(), O_RDONLY, 0);
        dup2(fd, STDIN_FILENO);
        //        in = 0;
        //        current_in = dup(0);  // Fix for symmetry with second paragraph
    }

    if ( out )
    { //if '>' was found in string inputted by user
        auto fd = creat(output.c_str(), 0644);
        dup2(fd, STDOUT_FILENO);
        //        out = 0;
        //        current_out = dup(1);
    }
}