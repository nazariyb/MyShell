#include "utils.h"



Args parse_arguments ( int argc, char * argv[] ){

    Args args;

    try{
        od desc {"Options"};
        desc.add_options()
                ("path", po::value<std::string>()->multitoken()->zero_tokens()->composing())
                ("help,h", po::bool_switch())
                (",p", po::bool_switch());

        po::positional_options_description pos_desc;
        pos_desc.add("path", -1);
        po::command_line_parser parser {argc, argv};
        auto parsed_options = parser
                .options(desc)
                .positional(pos_desc)
                .allow_unregistered()
                .run();

        po::variables_map vm;
        store(parsed_options, vm);
        notify(vm);


        if ( vm.count("path"))
            args.path = vm["path"].as<std::string>();

        args.p = vm["-p"].as<bool>();

    }
    catch ( const po::error & ex )
    {
        std::cerr << ex.what() << '\n';
        throw ex;
    }

    return args;

}

std::vector<std::string> split_path(std::string path, std::string delim){
    std::vector<std::string> result;

    size_t pos = 0;
    std::string token;

    while ((pos = path.find(delim)) != std::string::npos) {
        token = path.substr(0, pos);
        result.emplace_back(token + '/');
        path.erase(0, pos + delim.length());
    }

    result.emplace_back(path);

    return result;


}