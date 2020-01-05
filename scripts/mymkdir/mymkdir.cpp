#include "utils.h"

int main(int argc, char *argv[]) {
    Args args;

    try {
        args = parse_arguments(argc, argv);
    }
    catch (std::exception &ex) {
        _exit(PARSE_ARG_ERR);
    }

    if (args.help){
        std::cout << "mymkdir [-h|--help] [-p]  <dirname>\n"
                     "-h|--help  -- show this message\n"
                     "-p         -- create all necessary directories and don`t throw error in case of existing of directory\n"
                     "dirname    -- path to directory which you want to create" <<std::endl;
        _exit(SUCCESS);
    }


    if (args.p) {
        std::vector<std::string> dirs = split_path(args.path, "/");

        std::string current_path = dirs[0];
        for (int i = 1; i < dirs.size(); ++i) {

            current_path += dirs[i];

            if (!fs::exists((fs::path) current_path)) {
                fs::create_directory((fs::path) current_path);
            } else continue;
        }

    } else {
        fs::path full_path = args.path;

        if (fs::exists(full_path)) _exit(ERROR_EXIT);

        try {
            fs::create_directory(full_path);
        }
        catch (std::exception &ex) {
            _exit(ERROR_EXIT);
        }
    }


    _exit(SUCCESS);

}