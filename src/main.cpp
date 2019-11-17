#include <iostream>
#include <map>

#include <boost/filesystem.hpp>

#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>

#include "commands.h"
#include "utils.h"


EXIT_CODE last_exit_code{static_cast<EXIT_CODE>(0)};
int shell_exit_code{0};

VecStr PATH;
MapStrStr vars;
MapStrStr env_vars;

std::map<std::string, Command *> commands {
        {"merrno",  new mErrno {}},
        {"mpwd",    new mPwd {}},
        {"mcd",     new mCd {}},
        {"mexit",   new mExit {}},
        {"mecho",   new mEcho {}},
        {"mexport", new mExport {}},
        {".",       new mDot {}},
};


int main ( int argc, char * argv[] )
{
    PATH.emplace_back(fs::current_path().string() + "/../bin/");
    PATH.emplace_back("/bin/");
    PATH.emplace_back("/usr/bin/");

    if ( argc == 2 )
    {
        VecStr parsed_line {};
        parsed_line.emplace_back(".");
        parsed_line.emplace_back(argv[1]);
        commands["."]->run(parsed_line);

        for ( auto & command: commands )
            delete command.second;
        return shell_exit_code;
    }

    char *line;

    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    while ( true )
    {
        std::stringstream path{};
        path << green << "[myshell]:" << fs::current_path().string() << "$: " << def;
        line = readline(path.str().c_str());
        if ( !( line && ( *line ))) continue;

        add_history(line);
        if ( line[0] == '#' ) continue;

        auto parsed_line = parse_line(line);
        if ( parsed_line.size() > 1 )
            try_pipe(parsed_line, commands);
        else
        {
            auto command = parsed_line[0];
            if ( !commands.count(command[0]))
            {
                if ( try_to_execute(command) == SUCCESS )
                    continue;
                else if ( command.size() == 1 && try_add_var(command[0]) == SUCCESS )
                    continue;

                std::cerr << "\nmyshell: Command '" << command[0] << "' not found.\n" << endl;
                continue;
            }

            last_exit_code = commands[command[0]]->run(command);
        }

        if ( last_exit_code == FORCE_EXIT )
        {
            free(line);
            for ( auto & command: commands )
                delete command.second;
            return shell_exit_code;
        }

    }

}
