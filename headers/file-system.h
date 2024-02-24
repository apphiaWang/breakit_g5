#ifndef CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H
#define CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "cryptography.h"
#include "stringmanip.h"
#include <unistd.h>


class Filesystem {
private:
    std::unordered_map<std::string, std::filesystem::path> user_keyfiles;
    std::string username;
    std::filesystem::path base_directory;
    std::filesystem::path root_directory;

protected:
    void changeDirectory(const std::string &dir){
        if(dir.empty()){
            std::cout<<"Directory name not specified. "<<std::endl;
            return;
        }
        fs::path newPath;
        if(dir==".."){
            if(base_directory!=root_directory){
                newPath = base_directory.parent_path();
            } else {
                std::cout << "Already at the root directory. Cannot go up." << std::endl;
                return;
            }
        } else if (dir.front() == '/') {
            // Absolute path handling: navigate from root_directory
            newPath = root_directory / dir.substr(1);
        } else {
            // Relative path handling
            if(dir==".")
            {
                newPath=base_directory;
            }
            else{
                newPath = base_directory / dir;
            }
        }

        // Check if newPath is valid and within root_directory bounds
        if (fs::exists(newPath) && fs::is_directory(newPath) && (newPath.string().rfind(root_directory.string(), 0) == 0)) {
            base_directory = newPath;
        } else {
            std::cout << "Directory does not exist or is not accessible: " << dir << std::endl;
        }
    }

    static void list_directory_contents(const char *directory) {

        // Pointer to directory
        DIR *dir;

        // For each entry in the directory
        struct dirent *entry;

        // Open the current directory
        dir = opendir(directory);
        if (dir == nullptr) {
            std::cout << "Error opening directory" << std::endl;
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

    static void make_file(const std::string& make_file) {
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
            std::cout << "Unable to open the specified file" << std::endl;
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

    static void create_directory(const std::string &input) {
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

    static void cat_file(const std::string &filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            file.close();
        } else {
            std::cout << "Unable to open file: " << filename << std::endl;
        }
    }

    static void shareFile(const std::filesystem::path &source, const std::filesystem::path& sharedPath ,const std::string &sourcefile, const std::string &_username){
        std::filesystem::path destinationPath="./filesystem/"+_username+"/shared/" +sourcefile;
        std::filesystem::path sharedDirectory=sharedPath/sourcefile;
        try{
            std::filesystem::copy(source,destinationPath,std::filesystem::copy_options::overwrite_existing);
            std::filesystem::copy(source,sharedDirectory,std::filesystem::copy_options::overwrite_existing);
            std::filesystem::permissions(sharedDirectory, std::filesystem::perms::owner_read | std::filesystem::perms::group_read| std::filesystem::perms::others_read, std::filesystem::perm_options::replace);
            std::filesystem::permissions(destinationPath, std::filesystem::perms::owner_read | std::filesystem::perms::group_read| std::filesystem::perms::others_read, std::filesystem::perm_options::replace);
            std::cout<<"File has been copied to"<<destinationPath<<std::endl;
        }catch (const std::filesystem::filesystem_error &e){
            std::cout<<e.what()<<std::endl;
        }
    };

public:
    explicit Filesystem(const std::string &username, bool isAdmin=false, int count=0):username(username){
        base_directory="./filesystem";
        if(isAdmin) root_directory=base_directory;
        else    root_directory=base_directory/username;
        createFileSystem(username,count);
    }

    void createFileSystem(const std::string &_username, int count=0){
        base_directory="./filesystem/"+_username;
        if(count==1) {
            std::filesystem::create_directories(base_directory/".metadata/private_keys");
            //chmod((base_directory / "metadata/private_keys").c_str(), S_IRWXU);
        }
        std::filesystem::create_directories(base_directory/"personal");
        std::filesystem::create_directories(base_directory/"shared");
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
                createFileSystem(_username,1);
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
        } else if (command.substr(0,6)=="share "){
            std::vector _arr= splittext(command.substr(6),' ');
            std::vector source= splittext(_arr[0],'/'); //SourcePath - Assuming the path is ./filesystem/<user>/personal/<file.ext>
            std::string filename=source[source.size()/source[0].size()-1];
            std::string sharedPath="./"+source[1]+"/"+source[2]+"/shared";
            std::string originDirectory=_arr[0].substr(0,_arr[0].length()-filename.length());//Path is ./filesystem/<user>/personal/
            if(!std::filesystem::exists(originDirectory) || !std::filesystem::is_directory(originDirectory)){
                std::cout<<"Provided path is not a valid directory. "<<std::endl;
            }else{
                for(const auto&entry : std::filesystem::directory_iterator(originDirectory)){
                    if(entry.is_regular_file() && entry.path().filename()==filename){
                        shareFile(_arr[0],sharedPath,filename,_arr[1]);
                    }
                }
            }
        }else{
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

    std::string getCurrentWorkingDirectory(){
//        return base_directory.string().erase(0,1) + ">";
        try {
            // Canonicalize the base_directory to resolve any "..", ".", and symlinks
            fs::path canonicalBase = fs::canonical(base_directory);

            // Now, get the path relative to the root_directory
            fs::path relativePath = fs::relative(canonicalBase, root_directory);

            // Convert to string for display
            std::string displayPath = relativePath.string();

            // Check if the relativePath is just ".", indicating the root directory
            if (displayPath == ".") {
                displayPath = ""; // No need to show the dot for the root directory
            } else {
                // Prefix with '/' to maintain consistency in the display
                displayPath = "/" + displayPath;
            }

            // Ensure the display path is correctly prefixed for your root structure
            // and ensure it ends with ">", for your prompt display
            return "/filesystem" + displayPath + ">";
        } catch (const fs::filesystem_error& e) {
            // Handle potential error (e.g., base_directory not existing)
            std::cerr << "Error obtaining the current working directory: " << e.what() << std::endl;
            return "error>";
        }

//        try {
//            // Use the absolute path of the base_directory directly
//            fs::path absoluteBase = fs::canonical(base_directory); // This resolves "..", ".", and symlinks
//
//            // Convert the absolute path to a string for manipulation
//            std::string displayPath = absoluteBase.string();
//
//            // Ensure the display path does not contain the root_directory part for the display
//            std::string rootPath = fs::canonical(root_directory).string();
//            if(displayPath.rfind(rootPath, 0) == 0) {
//                // Erase the root_directory part to just have the path relative to root_directory
//                displayPath.erase(0, rootPath.length());
//            }
//
//            // Handling edge case for root ("/") to ensure consistency
//            if (displayPath.empty() || displayPath == "/") {
//                displayPath = "/filesystem>";
//            } else {
//                // Ensure the display path starts correctly for your desired format
//                displayPath = "/filesystem" + displayPath + ">";
//            }
//
//            return displayPath;
//        } catch (const fs::filesystem_error& e) {
//            // Handle potential error (e.g., base_directory or root_directory not existing)
//            std::cerr << "Error obtaining the current working directory: " << e.what() << std::endl;
//            return "error>";
//        }
    }
};

#endif //CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H
