#include <iostream>
#include "utils.h"

int main(int argc, char *argv[]) {
    Args args;

    try {
        args = parse_arguments(argc, argv);
    }
    catch (std::exception &ex) {
        _exit(PARSE_ARG_ERR);
    }


    if (args.help) {
        std::cout << "myrm [-h|--help] [-f] [-R] <file1> <file2> <file3>\n"
                     "Removes files. If recursive mode if setted - recursively remove directory.\n"
                     "-h|--help  -- show this message\n"
                     "-f         -- silent mode, if setted, you will not be asked of rewriting of file\n"
                     "-R         -- recursive mode" << std::endl;
        _exit(SUCCESS);
    }

    for (int i = 0; i < args.pathes.size(); i++){
        fs::path p(args.pathes[i]);

        if (fs::exists(p)) {

            if (fs::is_regular_file(p)) {

                if (!args.silent_mode) {
                    std::string answer;

                    std::cout << "Do you want to remove " << p.filename().string() << "?" << std::endl;
                    std::cout << "Y[es]/N[o]/A[ll]/C[ancel]" << std::endl;
                    std::cin >> answer;
                    boost::algorithm::to_lower(answer);
                    if (answer == "y") {
                        fs::remove(p);
                    }
                    else if (answer == "n") {
                        continue;
                    }
                    else if (answer == "a") {
                        args.silent_mode = true;
                        fs::remove(p);
                    }
                    else if (answer == "c") {
                        _exit(SUCCESS);
                    }
                }
                else {
                    fs::remove(p);
                }
            }

            else if (fs::is_directory(p)){
                if (args.recursive){
                    fs::remove_all(p);
                }
                else{
                    std::cout<<"You cannot remove directory without recursive mode on!"<<std::endl;
                    continue;
                }
            }

        }
        else _exit(ERROR_EXIT);

    }

    _exit(SUCCESS);
}