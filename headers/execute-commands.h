#ifndef CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H
#define CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H

#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>

// Create a namespace alias for convenience
namespace fs = std::filesystem;

void list_directory_contents() {

    // Pointer to directory
    DIR *dir;

    // For each entry in the directory
    struct dirent *entry;

    // To store information about each entry
    struct stat entry_stat;

    // Open the current directory
    dir = opendir(".");
    if (dir == nullptr) {
        std::cerr << "Error opening directory" << std::endl;
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {

        // Use stat to get information about the entry
        // Note: Using lstat instead of stat to correctly handle symbolic links
        lstat(entry->d_name, &entry_stat);

        // Check if the entry is a directory or a file
        if (S_ISDIR(entry_stat.st_mode)) {
            // It's a directory
            std::cout << "d -> " << entry->d_name << std::endl;
        } else {
            // It's a file
            std::cout << "f -> " << entry->d_name << std::endl;
        }
    }

    // Close the directory
    closedir(dir);
}

void print_working_directory() {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
}

void create_directory(const std::string& input) {
    // Extract directory name from input
    std::string command = input.substr(input.find(" ") + 1);

    // Check if directory name is empty
    if (command.empty()) {
        std::cout << "Directory not specified" << std::endl;
        return;
    }

    // Check if there are any invalid characters in the input for the directory name
    for (char c : command) {
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

void cat_file(const std::string& filename) {
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

void change_directory(const std::string& path) {

    // Extract directory name from input
    std::string command = path.substr(path.find(" ") + 1);

    // Check if directory name is empty
    if (path.empty()) {
        std::cout << "Directory not specified" << std::endl;
        return;
    }

    // Special case for cd /
    if (path == "/") {
        const char* home = getenv("HOME"); // POSIX systems
        if (home != nullptr) {
            if (chdir(home) == 0) {
                std::cout << "Moved to root directory." << std::endl;
            } else {
                std::cerr << "Failed to move to root directory." << std::endl;
            }
            return;
        }
    }

    // General case
    fs::path new_path = fs::current_path() / path;
    if (fs::exists(new_path) && fs::is_directory(new_path)) {
        if (chdir(new_path.c_str()) == 0) {
            std::cout << "Directory changed to " << new_path << std::endl;
        } else {
            std::cerr << "Failed to change directory." << std::endl;
        }
    } else {
        std::cout << "Directory does not exist." << std::endl;
    }
}

#endif //CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H
