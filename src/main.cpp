#include <iostream>
#include <map>

#include <boost/filesystem.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include "commands.h"
#include "utils.h"


EXIT_CODE last_exit_code{static_cast<EXIT_CODE>(0)};
int shell_exit_code{0};

VecStr PATH;
MapStrStr vars;
MapStrStr env_vars;


int main ()
{
    PATH.emplace_back(fs::current_path().string() + "/../bin/");
    PATH.emplace_back("/bin/");

    std::map<std::string, Command *> commands{
            {"merrno",  new mErrno{}},
            {"mpwd",    new mPwd{}},
            {"mcd",     new mCd{}},
            {"mexit",   new mExit{}},
            {"mecho",   new mEcho{}},
            {"mexport", new mExport{}},
    };

    char *line;

    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    while ( true )
    {
        std::stringstream path{};
        path << green << "[myshell]:" << fs::current_path().string() << "$: " << def;
        line = readline(path.str().c_str());
        if (!(line && (*line))) continue;

        add_history(line);

        auto parsed_line = parse_line(line);
        if ( !commands.count(parsed_line[0]))
        {
            if ( try_to_execute(parsed_line) == SUCCESS )
                continue;
            else if ( parsed_line.size() == 1 && try_add_var(parsed_line[0]) == SUCCESS )
                continue;

            std::cerr << "\nmyshell: Command '" << parsed_line[0] << "' not found.\n" << endl;
            continue;
        }

        last_exit_code = commands[parsed_line[0]]->run(parsed_line);

        if (last_exit_code == FORCE_EXIT) {
            free(line);
            for ( auto & command: commands )
                delete command.second;
            return shell_exit_code;
        }

    }

}
