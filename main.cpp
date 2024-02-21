#include <iostream>
#include "headers/menu-commands.h"
#include "headers/execute-commands.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::string command;
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
            //cd function
        } else if (command == "pwd") {
            print_working_directory();
        } else if (command == "ls") {
            list_directory_contents();
        } else if (command.substr(0, 4) == "cat ") {
            std::string filename = command.substr(4);
            cat_file(filename);
        } else {
            std::cout << "Unknown command. Please try again." << std::endl;
        }
    }

    return 0;
}
