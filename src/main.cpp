#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "commands.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <map>

#include "utils.h"


EXIT_CODE last_exit_code{static_cast<EXIT_CODE>(0)};
int shell_exit_code{0};

VecStr PATH;

std::map<std::string, std::string> env_vars;


int main() {

    PATH.push_back("../scripts/");
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
    while (true) {
        std::stringstream path{};
        path << green << "[myshell]:" << fs::current_path().string() << "$: " << def;
        line = readline(path.str().c_str());
        if (!(line && (*line))) continue;

        add_history(line);

        auto parsed_line = parse_line(line);

        if (!commands.count(parsed_line[0])) {

            try {
                run_if_in_path(parsed_line);
            }

            catch (std::exception &e) {
                std::cerr << "\nmyshell: Command '" << parsed_line[0] << "' not found.\n" << endl;
            }
            continue;
        }

        last_exit_code = commands[parsed_line[0]]->run(parsed_line);

        if (last_exit_code == FORCE_EXIT) {
            free(line);
            return shell_exit_code;
        }

    }


    std::map<std::string, std::string> m;

    m["a1"] = "1";
    m["b2"] = "2";
    m["c3"] = "3";

    std::vector<std::string> vec = map_to_VectStr(m);
    for (int j = 0; j < vec.size(); ++j) {
        std::cout << vec.at(j) << std::endl;
    }


}