#include <boost/program_options/positional_options.hpp>
#include <boost/algorithm/string.hpp>

#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <map>
#include <fnmatch.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "utils.h"
#include "commands.h"


bool run_in_bg {false};

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


VecVecStr parse_line ( const std::string & commandline )
{
    bool quotes_are_opened {false};
    bool brackets_are_opened {false};
    bool is_redirect {false};
    bool is_dollar {false};

    VecVecStr args {};
    std::string buff{};

    args.emplace_back();
    for ( auto & c: commandline )
    {
        if ( c == ' ' && buff.empty() && args.back().empty()) continue;
        if (c == '"') quotes_are_opened ^= static_cast<unsigned>(1);
        if ( c == '>' || c == '<' ) is_redirect = true;
        if ( c == '$' ) is_dollar = true;

        if ( c == '(' && is_dollar ) brackets_are_opened = true;

        if ( c == ')' && brackets_are_opened )
        {
            brackets_are_opened = false;
            is_dollar = false;
        }

        if ( c == '&' && is_redirect )
        {
            args.back().push_back(buff);
            buff.clear();
            buff += c;
            is_redirect = false;
            continue;
        }

        if ( c == '&' )
        {
            run_in_bg = true;
            return args;
        }

        if (( c == ' ' ) && ( !quotes_are_opened ) && ( !brackets_are_opened ))
        {
            args.back().push_back(buff);
            buff.clear();
            is_redirect = false;
            continue;
        }

        if (( c == '|' ) && ( !quotes_are_opened ) && ( !brackets_are_opened ))
        {
            args.emplace_back();
            buff.clear();
            continue;
        }

        buff += c;
    }
    args.back().push_back(buff);
    return args;
}


VecStr expand_arguments ( VecStr & arguments, Args & args, bool is_parsed = false )
{
    VecStr expanded_args {};

    if ( !is_parsed )
        Command::parse_redirections(arguments, args);

    for ( auto & arg: arguments )
    {
        auto vars = get_variable(arg);
        for ( auto & var: vars )
        {
            if ( var.empty() && variable_not_found )
                throw std::runtime_error("\nmyshell: Variable '"
                                         + arg.substr(1)
                                         + "' not found\n\n");
            expanded_args.push_back(var);
        }
    }

    return expanded_args;
}


int redirect ( const Args & args )
{
    static std::map<std::string, int> stdioe
            {
                    {"&0", STDIN_FILENO},
                    {"&1", STDOUT_FILENO},
                    {"&2", STDERR_FILENO}
            };

    int fd;
    if ( !args.input.empty())
    {
        if ( stdioe.count(args.input))
            dup2(stdioe[args.input], STDIN_FILENO);
        else
        {
            fd = open(args.input.c_str(), O_RDONLY);
            if ( !( fcntl(fd, F_GETFD) != -1 || errno != EBADF )) return -1;
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
    }
    //    else if (run_in_bg)
    //        close(STDIN_FILENO);
    if ( !args.output.empty())
    {
        if ( stdioe.count(args.output))
            dup2(stdioe[args.output], STDOUT_FILENO);
        else
        {
            // read & write for file's owner, read for group & others
            fd = creat(args.output.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if ( !( fcntl(fd, F_GETFD) != -1 || errno != EBADF )) return -1;
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
    }
    //    else if (run_in_bg)
    //        close(STDOUT_FILENO);
    if ( !args.error_output.empty())
    {
        if ( stdioe.count(args.error_output))
            dup2(stdioe[args.error_output], STDERR_FILENO);
        else
        {
            // read & write for file's owner, read for group & others
            if ( args.error_output == args.output )
                dup2(STDOUT_FILENO, STDERR_FILENO);
            else
            {
                fd = creat(args.error_output.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if ( !( fcntl(fd, F_GETFD) != -1 || errno != EBADF )) return -1;
                dup2(fd, STDERR_FILENO);
                close(fd);
            }
        }
    }
    //    else if (run_in_bg)
    //        close(STDERR_FILENO);

    return SUCCESS;
}


void fork_exec ( const std::string & exec_name, VecStr & arguments_ )
{

    //    pid_t parent = getpid();
    pid_t pid = fork();


    if ( pid == -1 )
        throw std::runtime_error("Failed to fork");
    else if ( pid > 0 )
    {
        if ( run_in_bg )
            return;
        int status;
        waitpid(pid, &status, 0);
        last_exit_code = static_cast<EXIT_CODE>(status);
    }
    else
    {
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


int try_to_execute ( VecStr command_opts )
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
    else
    {
        auto word = pair.substr(delim_ind + 1, std::string::npos);
        if ( boost::starts_with(word, "$(") && boost::ends_with(word, ")"))
        {
            auto parsed_line = parse_line(word.substr(2, word.size() - 3));
            if ( parsed_line.size() > 1 )
            {
                parsed_line.back().emplace_back(">");
                parsed_line.back().emplace_back(".var");
                try_pipe(parsed_line, commands);
            }
            else
            {
                auto command = parsed_line[0];
                command.emplace_back(">");
                command.emplace_back(".var");
                if ( !commands.count(command[0]))
                {
                    if ( try_to_execute(command) != SUCCESS )
                        //                            break;
                        //                        else if ( command.size() == 1 && try_add_var(command[0]) == SUCCESS )
                        //                            break;

                        std::cerr << "\nmyshell: Command '" << command[0] << "' not found.\n" << endl;
                }
                else
                {
                    last_exit_code = commands[command[0]]->run(command);
                }
            }
            auto var_fd = open(".var", O_RDONLY);
            auto len = lseek(var_fd, 0, SEEK_END);
            void * data = mmap(nullptr, len, PROT_READ, MAP_PRIVATE, var_fd, 0);
            vars[pair.substr(0, delim_ind)] = std::string {( char * ) data};
            close(var_fd);
        }
        else
            vars[pair.substr(0, delim_ind)] = word;
    }

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
            variable_not_found = true;
            return {std::string {""}};
        }
    }
    auto list = open_wildcard(word_);
    return ( list.empty()) ? VecStr {word_} : list;
}


int try_pipe ( VecVecStr & commands, std::map<std::string, Command *> & commands_map )
{
    auto numPipes = commands.size();

    int status;
    size_t i = 0;
    pid_t pid;

    int pipe_fds[2 * numPipes];

    for ( i = 0; i < ( numPipes ); i++ )
    {
        if ( pipe(pipe_fds + i * 2) < 0 )
        {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    for ( size_t i = 0, j = 0; i < numPipes; ++i )
    {
        pid = fork();
        if ( pid == 0 )
        {
            if ( i < numPipes - 1 )
            {
                if ( dup2(pipe_fds[j + 1], 1) < 0 )
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if ( j > 0 )
            {
                if ( dup2(pipe_fds[j - 2], 0) < 0 )
                {
                    perror(" dup2");
                    exit(EXIT_FAILURE);
                }
            }


            for ( size_t k = 0; k < 2 * numPipes; ++k )
            {
                close(pipe_fds[k]);
            }
            //             ==================
            VecStr arguments {};
            Args args_struct {};
            try
            {
                arguments = expand_arguments(commands[i], args_struct);
            }
            catch ( std::exception & ex )
            {
                throw ex;
            }

            if ( redirect(args_struct) != SUCCESS )
                throw std::runtime_error("unable to locate/create file for input/output");

            auto command_name = commands[i][0];
            std::string exec_name {};
            for ( auto & loc: PATH )
            {
                if ( boost::filesystem::exists(loc + command_name) & ( !access(( loc + command_name ).c_str(), X_OK)))
                {
                    exec_name = loc + command_name, commands[i];
                    break;
                }
            }
            if ( exec_name.empty() && !access(command_name.c_str(), X_OK))
            {
                exec_name = command_name;
            }

            arguments[0] = fs::path {exec_name}.filename().string();
            auto args = vecstr2char(arguments, true);
            auto env = vecstr2char(map2vecstr(env_vars), true);

            execve(exec_name.c_str(), args, env);

            delete[] args;
            delete[] env;
        }
        else if ( pid < 0 )
        {
            perror("error");
            exit(EXIT_FAILURE);
        }

        j += 2;

    }

    for ( i = 0; i < 2 * numPipes; i++ )
        close(pipe_fds[i]);

    if ( !run_in_bg )
        for ( i = 0; i < numPipes + 1; i++ )
            wait(&status);
}
