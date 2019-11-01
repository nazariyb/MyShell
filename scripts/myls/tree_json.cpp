#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>


namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

std::string modified_date ( const fs::path & );

pt::ptree * get_deep_child ( pt::ptree *, const fs::path &, const fs::path & );

void init_dir_node ( const fs::path &, pt::ptree & );

void init_file_node ( const fs::path &, pt::ptree & );


void print_tree ( const std::string & path )
{
    fs::path directory_name {path};

    pt::ptree root_directory;
    init_dir_node(directory_name, root_directory);

    for ( fs::recursive_directory_iterator end, dir(directory_name); dir != end; ++dir )
    {
        pt::ptree new_node;
        std::string node_type;

        if ( fs::is_directory(dir->path()))
        {
            init_dir_node(dir->path(), new_node);
            node_type = "Children";
        }
        else
        {
            init_file_node(dir->path(), new_node);
            node_type = "Files";
        }

        ( *get_deep_child(
                &root_directory,
                directory_name,
                ( *dir ).path().parent_path()
        ))
                .get_child(node_type)
                .push_back(std::make_pair("", new_node));
    }

    pt::write_json(std::cout, root_directory);
}


std::string modified_date ( const fs::path & directory_name )
{
    std::ostringstream ss;
    auto t = fs::last_write_time(directory_name);
    ss << std::put_time(std::localtime(&t), "%e-%b-%C %I:%M %p");
    return ss.str();
}


pt::ptree * get_deep_child ( pt::ptree * root_directory, const fs::path & directory_name, const fs::path & dir_path )
{
    std::string dir_path_in_tree {dir_path.generic_string()};
    std::string dir_name {directory_name.generic_string()};

    // convert path from type "C:/root_dir/sub_dir/sub_dir1"
    // to vector { "sub_dir", "sub_dir1" }
    // where root_dir is given path
    dir_path_in_tree.erase(dir_path_in_tree.find(dir_name),
                           dir_name.size() + static_cast<int>(dir_path_in_tree.size() > directory_name.size()));
    std::vector<std::string> path_as_vector;
    boost::split(path_as_vector, dir_path_in_tree, boost::is_any_of("/"));

    pt::ptree * searched_node = root_directory;

    // empty element means that node that is searched for is situtated in root directory
    if ( path_as_vector[0].empty()) return searched_node;

    // search for needed node, it is expected that it [node] exists
    while ( !path_as_vector.empty())
    {
        for ( auto & child: searched_node->get_child("Children"))
        {
            if ( child.second.get<std::string>("Name") == path_as_vector[0] )
            {
                path_as_vector.erase(path_as_vector.begin());
                searched_node = &child.second;
                break;
            }
        }
    }

    return searched_node;
}


void init_dir_node ( const fs::path & directory_name, pt::ptree & directory_node )
{
    directory_node.put("Name", directory_name.leaf().generic_string());
    directory_node.put("Date", modified_date(directory_name));
    directory_node.add_child("Files", pt::ptree {});
    directory_node.add_child("Children", pt::ptree {});
}


void init_file_node ( const fs::path & parent_name, pt::ptree & file_node )
{
    file_node.put("Name", parent_name.leaf().string());
    file_node.put("Size", fs::file_size(parent_name));
    file_node.put("Path", parent_name.generic_string());
}
