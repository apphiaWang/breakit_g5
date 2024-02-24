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
        if(dir=="personal" ||  dir=="shared" || dir=="sami"){
            base_directory/=dir;
            std::cout<<"Changed directory to "<<dir<<std::endl;
        }else{
            std::cout<<"Invalid Command"<<std::endl;
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
              //Function for mkdir or mkfile
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
};

#endif //CMPT785_G5_SECURE_FILESYSTEM_FILESYSTEM_H
