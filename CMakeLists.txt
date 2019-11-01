cmake_minimum_required(VERSION 3.10)
project(MyShell)

set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS OFF)

option(USE_BOOST_KARMA ON)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
add_definitions("-DUSE_BOOST_KARMA")
find_library(Readline_LIBRARY NAMES readline)

include_directories(${Readline_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

add_executable(myshell
        src/main.cpp
        src/commands.cpp
        src/commands.h
        src/utils.cpp
        src/utils.h)

target_link_libraries(myshell
        ${Boost_LIBRARIES}
        readline
        )

add_executable(mycat
        scripts/mycat/mycat.cpp)

target_link_libraries(mycat
        ${Boost_LIBRARIES}
        )
ADD_CUSTOM_COMMAND(
        TARGET mycat
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy mycat ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

add_executable(myls
        scripts/myls/myls.cpp
        scripts/myls/utils.cpp
        scripts/myls/utils.h
        scripts/myls/tree_json.cpp
        scripts/myls/tree_json.h
        scripts/myls/table_printer/table_printer.h
        scripts/myls/table_printer/table_printer.tpp.h
        scripts/myls/table_printer/table_printer.cpp
        )

target_link_libraries(myls
        ${Boost_LIBRARIES}
        )
ADD_CUSTOM_COMMAND(
        TARGET myls
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy myls ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
