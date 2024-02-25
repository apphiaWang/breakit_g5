cmake_minimum_required(VERSION 3.27)
project(cmpt785_g5_secure_filesystem)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

include_directories(include)

add_executable(cmpt785_g5_secure_filesystem main.cpp
        include/MenuCommands.h
        include/StringUtil.h

        include/Cryptography.h
        src/Cryptography.cpp

        include/FileSystem.h
        src/FileSystem.cpp
        src/StringUtil.cpp
)

target_link_libraries(cmpt785_g5_secure_filesystem ${OPENSSL_LIBRARIES})
