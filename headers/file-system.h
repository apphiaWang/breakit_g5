#ifndef CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H
#define CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "cryptography.h"


class Filesystem {
private:
    std::filesystem::path current_directory;
    std::unordered_map<std::string, std::filesystem::path> user_keyfiles;

public:
    Filesystem() : current_directory(std::filesystem::current_path()) {}

    void addUser(const std::string &username) {
        std::string keyfile_name;
        if (user_keyfiles.find(keyfile_name) == user_keyfiles.end()) {
            if (create_user_key(username)) {
                keyfile_name = username + "_pub.pem";
            }
            user_keyfiles[username] = std::filesystem::current_path() / keyfile_name;
            std::cout << "User " << username << " added successfully." << std::endl;
        } else {
            std::cout << "User " << username << " already exists." << std::endl;
        }
    }
};

#endif //CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H
