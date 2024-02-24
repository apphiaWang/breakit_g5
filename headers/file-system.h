#ifndef CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H
#define CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "cryptography.h"
#include "stringmanip.h"
#include <sys/stat.h>
#include <unistd.h>


class Filesystem {
private:
    std::unordered_map<std::string, std::filesystem::path> user_keyfiles;
    std::string username;
    std::filesystem::path base_directory;

protected:
    void changeDirectory(const std::string &dir){

        if (dir.empty()) {
            std::cout << "Directory name not specified." << std::endl;
            return;
        }

//        if(dir=="personal" ||  dir=="shared" || dir=="sami"){
//            base_directory/=dir;
//            std::cout<<"Changed directory to "<<dir<<std::endl;
//        }else{
//            std::cout<<"Invalid Command"<<std::endl;
//        }

        if (dir == "/") {
            const char *home = getenv("HOME");
            if (home && chdir(home) == 0) {
                std::cout << "Moved to home directory." << std::endl;
                return;
            } else {
                std::cerr << "Failed to move to home directory." << std::endl;
                return;
            }
        }

        fs::path newPath;
        std::vector<std::string> components = split(dir, '/');
        bool isAbsolute = dir.front() == '/';

        newPath = isAbsolute ? fs::path(getenv("HOME")) : fs::current_path(); // Adjust for root or current path

        for (const auto &component : components) {
            auto nextPath = newPath / component;
            if (fs::exists(nextPath) && fs::is_directory(nextPath)) {
                newPath = nextPath;
            } else {
                std::cout << "Directory does not exist or case mismatch: " << component << std::endl;
                return;
            }
        }

        if (chdir(newPath.c_str()) == 0) {
            std::cout << "Directory changed to " << newPath << std::endl;
        } else {
            std::cerr << "Failed to change directory to " << newPath << std::endl;
        }
    }

    void list_directory_contents(const char *directory) {

        // Pointer to directory
        DIR *dir;

        // For each entry in the directory
        struct dirent *entry;

        // Open the current directory
        dir = opendir(directory);
        if (dir == nullptr) {
            std::cerr << "Error opening directory" << std::endl;
            return;
        }

        while ((entry = readdir(dir)) != nullptr) {
            // Check if the entry is a directory or a file
            if (entry->d_type == DT_DIR) {
                // It's a directory
                std::cout << "d -> " << entry->d_name << std::endl;
            } else if (entry->d_type == DT_REG) {
                // It's a regular file
                std::cout << "f -> " << entry->d_name << std::endl;
            } else {
                // For other types, you can either ignore or print a generic type
                std::cout << "? -> " << entry->d_name << std::endl;
            }
        }

        // Close the directory
        closedir(dir);
    }

    void make_file(const std::string& make_file) {
        // Extract filename and contents from input
        std::istringstream iss(make_file);
        std::string command, filename, contents;
        bool flag= false;

        flag = 0;
        iss >> command >> filename; // Read command and filename
        std::getline(iss >> std::ws, contents); // Read contents, including whitespace

        // Trim leading and trailing whitespace from filename and contents
        filename.erase(0, filename.find_first_not_of(" \t"));
        filename.erase(filename.find_last_not_of(" \t") + 1);
        contents.erase(0, contents.find_first_not_of(" \t"));
        contents.erase(contents.find_last_not_of(" \t") + 1);

        // Check if both filename and contents are missing
        if (filename.empty() && contents.empty()) {
            std::cout << "<filename> and <contents> arguments are missing, please try again" << std::endl;
            return;
        }

        // Check if filename is empty
        if (filename.empty()) {
            std::cout << "Filename not specified" << std::endl;
            return;
        }

        // Check to see if any invalid characters exist in the filename
        for (char c : filename) {
            if (std::isspace(c) || c == '?' || c == ':' || c == '\\' || c == '*' || c == '/' || c == '"' || c == '|') {
                std::cout << "Invalid characters added to the filename, please re-enter" << std::endl;
                return;
            }
        }

        // Check if contents are missing
        if (contents.empty()) {
            std::cout << "<content> argument is missing, please try again" << std::endl;
            return;
        }
        // Check if the file already exists
        if (fs::exists(filename)) {
            std::cout << "The file '" << filename << "' already exists. It will be overwritten." << std::endl;
            flag = true;
        }

        // Create or open the file
        std::ofstream mkfile(filename);

        // Check if file is opened successfully
        if (!mkfile.is_open()) {
            std::cerr << "Unable to open the specified file" << std::endl;
            return;
        }

        // Write contents to the file
        mkfile<< contents;
        mkfile.close();

        // Output success message based on whether the file was created or modified
        if (flag) {
            std::cout << "File is modified successfully:  " << filename << std::endl;
        } else {
            std::cout << "File is created successfully: " << filename << std::endl;
        }
    }

    void create_directory(const std::string &input) {
        // Extract directory name from input
        std::string command = input.substr(input.find(" ") + 1);

        // Check if directory name is empty
        if (command.empty()) {
            std::cout << "Directory not specified" << std::endl;
            return;
        }

        // Check if there are any invalid characters in the input for the directory name
        for (char c: command) {
            if (std::isspace(c) || c == '?' || c == ':' || c == '\\' || c == '*' || c == '/' || c == '"' || c == '|') {
                std::cout << "Invalid characters added to the directory filename, please re-enter" << std::endl;
                return;
            }
        }

        // Check if directory already exists
        if (fs::exists(command)) {
            std::cout << "Directory already exists" << std::endl;
            return;
        }

        // Create directory
        fs::create_directory(command);
        std::cout << "Directory created successfully" << std::endl;
    }

    void cat_file(const std::string &filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            file.close();
        } else {
            std::cerr << "Unable to open file: " << filename << std::endl;
        }
    }

public:
    explicit Filesystem(const std::string &username,bool isAdmin=false, int count=0):username(username){
        createFileSystem(username,isAdmin,count);
    }

    void createFileSystem(const std::string &_username, bool isAdmin=false, int count=0){
        if(isAdmin){
            base_directory=std::filesystem::current_path()/"filesystem";
            if(count==1) {
                std::filesystem::create_directories(base_directory / "metadata/private_keys");
                chmod((base_directory / "metadata/private_keys").c_str(), S_IRWXU);
            }
            std::filesystem::create_directories(base_directory/"personal");
            std::filesystem::create_directories(base_directory/"shared");
        }else{
            base_directory=std::filesystem::current_path()/"filesystem"/_username;
            if(count==1) {
                std::filesystem::create_directories(base_directory / "metadata/private_keys");
                chmod((base_directory / "metadata/private_keys").c_str(), S_IRWXU);
            }
            std::filesystem::create_directories(base_directory/"personal");
            std::filesystem::create_directories(base_directory/"shared");
        }
    }

    void processUserCommand(const std::string &command, bool isAdmin) {
        if(command.substr(0,3)=="cd "){
            changeDirectory(command.substr(3));
        }
        else if(command.substr(0,2)=="ls"){
            list_directory_contents((base_directory).c_str());
        }
        else if(command=="pwd"){
            std::cout<<base_directory<<std::endl;
        }else if(command.substr(0,8)=="adduser "){
            if(isAdmin){
                std::vector<std::string>str= splittext(command,' ');
                std::string _username=str[1];
                createFileSystem(_username, false,1);
                addUser(_username);
            }else{
                std::cout<<"Invalid Command"<<std::endl;
            }
        }else if (command.substr(0,6)=="mkdir " || command.substr(0,7)=="mkfile "){
            if(isAdmin){
                std::cout<<"Forbidden"<<std::endl;
            }else{
              if(command.substr(0, 6) == "mkdir ") {
                  create_directory(command);
              }
              if(command.substr(0,7) == "mkfile ") {
                  make_file(command);
              }
            }
        } else if (command.substr(0, 4) == "cat ") {
            std::string filename = command.substr(4);
            cat_file(filename);
        } else{
            std::cout<<"Invalid Command"<<std::endl;
        }
    }

    static void addUser(const std::string &_username, bool isAdmin=false) {
        std::string publicKeyPath="public_keys/" + _username + "_pub.pem";
        if (!std::filesystem::exists(publicKeyPath)) {
            if (create_user_key(_username,isAdmin)){
                std::cout << "User " << _username << " added successfully." << std::endl;
            }
        } else {
            std::cout << "User " << _username << " already exists." << std::endl;
        }
    }
};

#endif //CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H


/*
 * 1 - visibility, structure, location, root directory /
 *      cannot create files in root (shared, meta)
 *      cannot create directories in root /
 * filesystem
 *      admin /
 *          meta
 *          personal
 *          shared
 *      u1 /
 *          meta
 *          personal
 *          shared
 * 2
 *  functions (cd, ls, pwd)
 *
 */