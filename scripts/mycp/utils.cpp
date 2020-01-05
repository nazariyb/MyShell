#include "utils.h"


Args parse_arguments ( int argc, char * argv[] ){

    Args args;

    try{
        od desc {"Options"};
        desc.add_options()
                ("pathes", po::value<VecStr>()->multitoken()->zero_tokens()->composing())
                (",-", po::value<VecStr>()->multitoken()->zero_tokens()->composing())
                ("help,h", po::bool_switch())
                (",f", po::bool_switch())
                (",R", po::bool_switch());
        po::positional_options_description pos_desc;
        pos_desc.add("pathes", -1);
        po::command_line_parser parser {argc, argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .allow_unregistered()
                .run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);


        if ( vm.count("pathes"))
            args.pathes = vm["pathes"].as<VecStr>();


        args.help = vm["help"].as<bool>();

        args.silent_mode = vm["-f"].as<bool>();

        args.recursive = vm["-R"].as<bool>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        throw ex;
    }

    return args;

}


bool my_copy_file(const fs::path &src, const fs::path &dst, bool silent_mode){
    if (fs::exists(dst)){

        if (!silent_mode){
            std::string answer;

            std::cout<<"Do you want to rewrite "<<src.filename().string()<<"?"<<std::endl;
            std::cout<<"Y[es]/N[o]/A[ll]/C[ancel]"<<std::endl;
            std::cin>> answer;
            boost::algorithm::to_lower(answer);
            if (answer == "y"){
                fs::copy_file(src, dst, fs::copy_option::overwrite_if_exists);
            }
            else if (answer == "a"){
                silent_mode = true;
            }
            else if (answer == "c"){
                exit(0);
            }
        }
        else{
            fs::copy_file(src, dst, fs::copy_option::overwrite_if_exists);
        }
    }

    else{
        fs::copy(src, dst);
    }

    return silent_mode;
}

void recursive_copy(const fs::path &src, const fs::path &dst, bool sm){
    if (fs::is_directory(src)) {
        fs::create_directories(dst);
        for (fs::directory_entry& item : fs::directory_iterator(src)) {
            recursive_copy(item.path(), dst/item.path().filename(), sm);
        }
    }
    else if (fs::is_regular_file(src)) {
        try {
            sm = my_copy_file(src, dst, sm);
        }
        catch (std::exception &ex){
            _exit(ERROR_EXIT);
        }

    }
}
