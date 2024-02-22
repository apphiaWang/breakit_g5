#ifndef CMPT785_G5_SECURE_FILESYSTEM_MENU_COMMANDS_H
#define CMPT785_G5_SECURE_FILESYSTEM_MENU_COMMANDS_H

#include <iostream>
#include <iomanip>

void available_commands() {

    // Width for the command column
    int commandWidth = 40;
    // Width for the description column
    int descriptionWidth = 80;

    std::cout << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "Available Commands:" << std::setw(descriptionWidth) << std::left << "Description" << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "------------------" << std::setw(descriptionWidth) << std::left << "--------------------" << std::endl;

    std::cout << std::setw(commandWidth) << std::left << "cd <directory>" << std::setw(descriptionWidth) << std::left << "Change directory" << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "pwd" << std::setw(descriptionWidth) << std::left << "Print the current working directory" << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "ls" << std::setw(descriptionWidth) << std::left << "List the files and directories in the current directory" << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "cat <filename>" << std::setw(descriptionWidth) << std::left << "Display the content of the given filename" << std::endl;

    std::cout << std::setw(commandWidth) << std::left << "share <filename> <username>" << std::setw(descriptionWidth) << std::left << "Share the file <filename> with the target user <username>" << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "mkdir <directory>" << std::setw(descriptionWidth) << std::left << "Create a new directory" << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "mkfile <filename> <contents>" << std::setw(descriptionWidth) << std::left << "Create a new file <filename> with the ascii printable contents <contents>" << std::endl;

    std::cout << std::setw(commandWidth) << std::left << "adduser <username>" << std::setw(descriptionWidth) << std::left << "Add new user by the given <username>" << std::endl;
    std::cout << std::setw(commandWidth) << std::left << "exit" << std::setw(descriptionWidth) << std::left << "Terminate the program" << std::endl;
}

#endif //CMPT785_G5_SECURE_FILESYSTEM_MENU_COMMANDS_H