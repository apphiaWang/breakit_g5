cmake_minimum_required(VERSION 3.27)
project(cmpt785_g5_secure_filesystem)

set(CMAKE_CXX_STANDARD 17)

add_executable(cmpt785_g5_secure_filesystem main.cpp
        headers/menu-commands.h
        headers/execute-commands.h)
