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
        std::cout << "mymv [-h|--help] [-f] <oldfile> <newfile>\n"
                     "Renames oldfile to newfile\n\n"
                     "mymv [-h|--help] [-f] <oldfile_or_dir_1> <oldfile_or_dir_oldfile2> "
                     "<oldfile_or_dir_oldfile3>.... <dir>\n"
                     "Moves all files and directories to dir\n\n"
                     "-h|--help  -- show this message\n"
                     "-f         -- silent mode, if setted, you will not be asked of rewriting of file\n"<< std::endl;
        _exit(SUCCESS);
    }


    // in case of renaming first file to second
    if (args.pathes.size() == 2 &&
        fs::is_regular_file(args.pathes[0]) &&
        fs::is_regular_file(args.pathes[1]))
    {
        args.silent_mode = my_move_file((fs::path) args.pathes[0], (fs::path) args.pathes[1], args.silent_mode);
    }

    if (fs::is_directory(args.pathes.back())) {
        for (int i = 0; i < args.pathes.size() - 1; i++) {
            fs::path p(args.pathes[i]);


            if (fs::is_regular_file(args.pathes[i])) {
                fs::path targ_file(args.pathes.back() + "/" + p.filename().string());

                args.silent_mode = my_move_file(p, targ_file, args.silent_mode);
            }

            else if (fs::is_directory(args.pathes[i])){

                std::string pp =  p.parent_path().string();
                size_t indx = pp.find_last_of("/\\");
                auto folder = pp.substr(indx+1);

                fs::create_directory((fs::path) args.pathes.back()/folder);
                try{

                    args.silent_mode = my_move_file(args.pathes[i], (fs::path) args.pathes.back()/folder, args.silent_mode);
                }
                catch (std::exception &ex){
                    _exit(ERROR_EXIT);
                }
            }

        }
    }


    _exit(SUCCESS);




}