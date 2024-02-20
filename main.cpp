#include <iostream>
#include "headers/menu-commands.h"

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
            // pwd function
        } else if (command == "ls") {
            // ls function
        } else if (command.substr(0, 4) == "cat ") {
            // cat function
        } else {
            std::cout << "Unknown command. Please try again." << std::endl;
        }
    }

    return 0;
}
