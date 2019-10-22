#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "commands.h"

#include <readline/readline.h>
#include <readline/history.h>

#include "utils.h"


EXIT_CODE last_exit_code {static_cast<EXIT_CODE>(0)};


std::vector<std::string> parse_line ( const std::string & commandline )
{
    bool quotes_are_opened {false};
    std::vector<std::string> args {};
    std::string buff {};
    for ( auto & c: commandline )
    {
        if ( c == '"' ) quotes_are_opened ^= 1;

        if (( c == ' ' ) && ( !quotes_are_opened ))
        {
            args.push_back(buff);
            buff.clear();
            continue;
        }
        buff += c;
    }
    args.push_back(buff);
    return args;
}


int main ()
{
    std::map<std::string, Command *> commands {
            {"merrno",  new mErrno {}},
            {"mpwd",    new mPwd {}},
            {"mcd",     new mCd {}},
            {"mexit",   new mExit {}},
            {"mecho",   new mEcho {}},
            {"mexport", new mExport {}},
    };

    char * line;

    while ( true )
    {

        line = readline(std::string {"[myshell]:" + fs::current_path().string() + "$: "}.c_str());
        if ( !( line && ( *line ))) continue;

        add_history(line);

        auto parsed_line = parse_line(line);

        try
        {
            last_exit_code = commands[parsed_line[0]]->run(parsed_line);
        }
        catch ( Exit & ex )
        {
            free(line);
            return ex.what();
        }
        catch ( std::exception & ex )
        {
            std::cerr << "Error ocurred: " << ex.what() << endl;
        }

    }

}