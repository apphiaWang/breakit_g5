#include <iostream>
#include <vector>
#include "headers/menu-commands.h"
#include "headers/execute-commands.h"
#include "headers/file-system.h"
#include "headers/cryptography.h"
#include "headers/stringmanip.h"

Filesystem f;

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::string command;
    bool shouldExit = false;
    std::string usertype = "admin";

    // Authentication function
    available_commands();
    if (usertype == "user") {
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
            } else if (command.substr(0, 6) == "mkdir ") {
                create_directory(command);
            } else {
                std::cout << "Unknown command. Please try again." << std::endl;
            }
        }
    } else if (usertype == "admin") {
        while (!shouldExit) {
            std::cout << "\nEnter command: ";
            std::getline(std::cin, command);
            if (command == "exit") {
                // terminate function
                shouldExit = true;
            } else if (command.substr(0, 8) == "adduser ") {
                std::vector<std::string> str = splittext(command, ' ');
                std::string username = str[1];
                f.addUser(username);

                std::string result = encrypt_plaintext("Hello World", username);
                std::cout << "Encrypted data:" + result << std::endl;
                std::cout << "Decrypted data: " + decrypt_ciphertext(result, username + "_priv.pem") << std::endl;
            } else {
                std::cout << "Unknown command. Please try again." << std::endl;
            }
        }
    }
    return 0;
}