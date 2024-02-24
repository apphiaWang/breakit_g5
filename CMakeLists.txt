cmake_minimum_required(VERSION 3.27)
project(cmpt785_g5_secure_filesystem)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

add_executable(cmpt785_g5_secure_filesystem main.cpp
        headers/menu-commands.h
        headers/execute-commands.h
        headers/cryptography.h
        headers/file-system.h
        headers/stringmanip.h
        headers/stringmanip.h
)

target_link_libraries(cmpt785_g5_secure_filesystem ${OPENSSL_LIBRARIES})