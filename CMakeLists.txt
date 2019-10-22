cmake_minimum_required(VERSION 3.10)
project(MyShell)

set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS OFF)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_library(Readline_LIBRARY NAMES readline)

include_directories(${Readline_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

add_executable(myshell src/main.cpp src/commands.cpp src/commands.h src/utils.cpp src/utils.h)

target_link_libraries(myshell
        ${Boost_LIBRARIES}
        readline
        )
