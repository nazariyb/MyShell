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
        std::cout << "mycp [-h|--help] [-f] <file1> <file2>\n"
                     "Copies from file1 to file2, file1 cannot be a directory\n\n"
                     "mycp [-h|--help] [-f]  <file1> <file2> <file3>... <dir>\n"
                     "Copies all files to dir\n\n"
                     "mycp [-h|--help] [-f] -R  <dir_or_file_1> <dir_or_file_2> <dir_or_file_3>... <dir>\n"
                     "Recursively copies all file or dirs to dir\n\n"
                     "-h|--help  -- show this message\n"
                     "-f         -- silent mode, if setted, you will not be asked of rewriting of file\n"
                     "-R         -- recursive mode" << std::endl;
        _exit(SUCCESS);
    }

    //exit if in sources there is directory, but target is file
    // !!! works only if target file exists
    if ((fs::is_directory(args.pathes[0]) && fs::is_regular_file(args.pathes.back()))) {
        std::cout << "Can not copy directory to the file!" << std::endl;
        _exit(FILE_TYPE_ERR);
    }


    for (int i = 0; i < args.pathes.size() - 1; i++) {
        std::string targ_file = args.pathes.back();
        fs::path p(args.pathes[i]);

        // if we copying file
        if (fs::is_directory(args.pathes.back()) && fs::is_regular_file(args.pathes[i])) {
            //add name of source file to path of target dir
            targ_file = args.pathes.back() + "/" + p.filename().string();
        }

        // if we copying directory
        if (fs::is_directory(args.pathes[i])) {
            if (args.recursive) {
                //create last directory from source in target dir to copy there all other files from source
                try {
                    fs::create_directory((fs::path) args.pathes.back() / p.filename());
                    recursive_copy(args.pathes[i], args.pathes.back() / p.filename(), args.silent_mode);
                }
                catch (std::exception &ex) {
                    _exit(ERROR_EXIT);
                }
            }
                // cannot copy without recursive mode on
            else {
                _exit(FILE_TYPE_ERR);
            }
        }

        //copy file
        try {
            args.silent_mode = my_copy_file(p, targ_file, args.silent_mode);
        }
        catch (std::exception &ex) {
            _exit(ERROR_EXIT);
        }
    }

    _exit(SUCCESS);
}