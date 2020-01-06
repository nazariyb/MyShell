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

# MYCAT
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

# MYLS
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

# MYCP
add_executable(mycp
        scripts/mycp/mycp.cpp
        scripts/mycp/utils.cpp
        scripts/mycp/utils.h
        )

target_link_libraries(mycp
        ${Boost_LIBRARIES}
        )
ADD_CUSTOM_COMMAND(
        TARGET mycp
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy mycp ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# MYGREP
add_executable(mygrep
        scripts/mygrep/mygrep.cpp
        )

target_link_libraries(mygrep
        ${Boost_LIBRARIES}
        )
ADD_CUSTOM_COMMAND(
        TARGET mygrep
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy mygrep ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# MYMKDIR
add_executable(mymkdir
        scripts/mymkdir/mymkdir.cpp
        scripts/mymkdir/utils.cpp
        scripts/mymkdir/utils.h
        )

target_link_libraries(mymkdir
        ${Boost_LIBRARIES}
        )
ADD_CUSTOM_COMMAND(
        TARGET mymkdir
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy mymkdir ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# MYMV
add_executable(mymv
        scripts/mymv/mymv.cpp
        )

target_link_libraries(mymv
        ${Boost_LIBRARIES}
        )
ADD_CUSTOM_COMMAND(
        TARGET mymv
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy mymv ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# MYRM
add_executable(myrm
        scripts/myrm/myrm.cpp
        scripts/myrm/utils.cpp
        scripts/myrm/utils.h
        )

target_link_libraries(myrm
        ${Boost_LIBRARIES}
        )
ADD_CUSTOM_COMMAND(
        TARGET myrm
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy myrm ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
