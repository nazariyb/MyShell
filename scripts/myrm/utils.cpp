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

