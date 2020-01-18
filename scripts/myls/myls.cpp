#include <iostream>

#include "utils.h"
#include "tree_json.h"


int main ( int argc, char * argv[] )
{
    Args args;
    try
    {
        args = parse_arguments(argc, argv);
    }
    catch ( std::exception & ex )
    {
        return PARSE_ARGUMENTS_ERROR;
    }

    if ( args.help )
    {
        cout << "myls [path|mask] [-l] [-h|--help] [--sort=U|S|t|N|X|D|s] [-r] [-R] [-F] [--tree]\n"
                "path       -- path or wildcard mask to file(s)\n"
                "-l         -- show additional info about files\n\n"
                "-h|--help  -- show this message\n\n"
                "--sort=AB\n"
                "       A is from [U, S, t, N, X]\n"
                "           U - don't sort\n"
                "           S - sort by size\n"
                "           t - sort by last time of modification\n"
                "           N - sort by name (default)\n"
                "           X - sort by extension\n"
                "       B is from [D, s] - additional argument\n"
                "           D - print directories first\n"
                "           s - print special files first\n\n"
                "-r         -- reverse order of sorting\n\n"
                "-R         -- iterate through subdirectories (recursively)\n\n"
                "-F         -- print type of file:\n"
                "           * - executable\n"
                "           @ - symlink\n"
                "           | - channel\n"
                "           = - socket\n"
                "           ? - others\n\n"
                "--tree     -- fun feature\n"
                "           Ignores all other arguments and prints directory with all its subdirectories\n"
                "           and their ones..., with some additional info in format of json " << endl;
        return HELP_EXIT;
    }

    if ( args.tree )
    {
        print_tree(args.path[0]);
        return PRINT_TREE;
    }

    MapVecFile files;
    try
    {
        files = get_files(args.path, args.recursive);
    }
    catch ( std::exception & ex )
    {
        cerr << "myls: " << ex.what() << endl;
    }

    try
    {
        sort_files(files, args);
    }
    catch ( std::exception & ex )
    {
        cerr << "myls: " << ex.what() << endl;
        return SORT_ERROR;
    }

    try
    {
        print_files(files, args);
    }
    catch ( std::exception & ex )
    {
        cerr << "myls: " << ex.what() << endl;
        return PRINT_FILES_ERROR;
    }

    return exit_code;
}

