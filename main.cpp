
//int main() {
//    std::cout << "Hello, World!" << std::endl;
//    std::string command;
//    bool shouldExit = false;
//    std::string usertype = "admin";
//
//    // Authentication function
//    available_commands();
//    if (usertype == "user") {
//        while (!shouldExit) {
//            std::cout << "\nEnter command: ";
//            std::getline(std::cin, command);
//
//            if (command == "exit") {
//                // terminate function
//                shouldExit = true;
//            } else if (command.substr(0, 3) == "cd ") {
//                std::string dir = command.substr(3);
//                change_directory(dir);
//            } else if (command == "pwd") {
//                print_working_directory();
//            } else if (command == "ls") {
//                list_directory_contents();
//            } else if (command.substr(0, 4) == "cat ") {
//                std::string filename = command.substr(4);
//                cat_file(filename);
//            } else if (command.substr(0, 6) == "mkdir ") {
//                create_directory(command);
//            } else {
//                std::cout << "Unknown command. Please try again." << std::endl;
//            }
//        }
//    } else if (usertype == "admin") {
//        while (!shouldExit) {
//            std::cout << "\nEnter command: ";
//            std::getline(std::cin, command);
//            if (command == "exit") {
//                // terminate function
//                shouldExit = true;
//            } else if (command.substr(0, 8) == "adduser ") {
//                std::vector<std::string> str = splittext(command, ' ');
//                std::string username = str[1];
//                f.addUser(username);
//
//                std::string result = encrypt_plaintext("Hello World", username);
//                std::cout << "Encrypted data:" + result << std::endl;
//                std::cout << "Decrypted data: " + decrypt_ciphertext(result, username + "_priv.pem") << std::endl;
//            } else {
//                std::cout << "Unknown command. Please try again." << std::endl;
//            }
//        }
//    }
//    return 0;
//}

#include <iostream>
#include "headers/menu-commands.h"
#include "headers/execute-commands.h"
<<<<<<< HEAD
=======
#include "headers/file-system.h"
#include "headers/cryptography.h"
#include "headers/stringmanip.h"

void initializeFileSystem(){
    std::filesystem::create_directories("public_keys");
    std::filesystem::create_directories("filesystem/personal");
    std::filesystem::create_directories("filesystem/shared");
}
>>>>>>> 8e5de31 (Added implementation of terminal)

bool authenticateUser(const std::string &username){
    std::string publicKeyPath="public_keys/" + username + "_pub.pem";
    if (!std::filesystem::exists(publicKeyPath)){
        return false;
    }else {
        std::string message = "Hello";
        std::string result;
        if (decrypt_ciphertext(encrypt_plaintext(message, username), username + "_priv.pem") == message) {
            return true;
        }
    }
    return false;
}

void processCommand(const std::string& command, Filesystem &fs, bool _isAdmin){
    if(command=="exit"){
        std::cout<<"Exiting..."<<std::endl;
        exit(1);
    }else{
        fs.processUserCommand(command, _isAdmin);
    }
}


int main(int argc, char** argv){
    bool isAdmin= false;
    if(argc!=2){
        std::cerr<<"Usage: ./fileserver <username_keyfile>"<<std::endl;
        return 1;
    }
    std::string username_keyfile=argv[1];
    std::string username= splittext(username_keyfile,'_')[0];
    initializeFileSystem();

    if(username.find("admin")!=std::string::npos)   isAdmin=true;

    if(!authenticateUser(username)){
        if(isAdmin){
            Filesystem fs(username,isAdmin,1);
            Filesystem::addUser(username,isAdmin);
        }else{
        std::cerr<<"Invalid keyfile"<<std::endl;
        return 1;
        }
    }

    std::cout<<"Logged in as " + username<<std::endl;
    Filesystem fs(username,isAdmin);
    std::string command;
<<<<<<< HEAD
    bool shouldExit = false;

    // Authentication function
    available_commands();

    while (!shouldExit) {
        std::cout << "\nEnter command: ";
        std::getline(std::cin, command);

        if (command == "exit") {
            // terminate function
            shouldExit = true;
        } else if (command.substr(0, 3) == "cd ") {
            std::string dir = command.substr(3);
            change_directory(dir);
        } else if (command == "pwd") {
            print_working_directory();
        } else if (command == "ls") {
            list_directory_contents();
        } else if (command.substr(0, 4) == "cat ") {
            std::string filename = command.substr(4);
            cat_file(filename);
        } else if (command.substr(0, 7) == "mkfile ") {
            make_file(command);
        } else if (command.substr(0, 6) == "mkdir ") {
            create_directory(command);
        } else {
            std::cout << "Unknown command. Please try again." << std::endl;
        }
=======
    while(true){
        std::cout<<username<<"@filesystem> ";
        std::getline(std::cin,command);
        processCommand(command, fs , isAdmin);
>>>>>>> 8e5de31 (Added implementation of terminal)
    }
    return 0;
}
