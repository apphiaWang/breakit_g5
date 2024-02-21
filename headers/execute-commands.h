#ifndef CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H
#define CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H

#include <iostream>
#include <filesystem>
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

void make_file(const std::string& make_file) {
    // Extract filename and contents from input
    std::istringstream iss(make_file);
    std::string command, filename, contents;
    iss >> command >> filename; // Read command and filename
    std::getline(iss >> std::ws, contents); // Read contents, including whitespace

    // Trim leading and trailing whitespace from filename and contents
    filename.erase(0, filename.find_first_not_of(" \t"));
    filename.erase(filename.find_last_not_of(" \t") + 1);
    contents.erase(0, contents.find_first_not_of(" \t"));
    contents.erase(contents.find_last_not_of(" \t") + 1);

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

    std::cout << "File is created successfully, called: " << filename << std::endl;
}
#endif //CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H
