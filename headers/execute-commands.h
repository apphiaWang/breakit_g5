#ifndef CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H
#define CMPT785_G5_SECURE_FILESYSTEM_EXECUTE_COMMANDS_H

#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <vector>

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

// Utility function to split a string by delimiter into a vector of strings
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Check if a single directory component matches exactly in the given directory
bool isExactComponentMatch(const fs::path& base, const std::string& component) {
    if (component == "." || component == ".." || component.empty()) {
        return true; // These are always considered valid
    }

    for (const auto& entry : fs::directory_iterator(base)) {
        if (entry.is_directory() && entry.path().filename() == component) {
            return true; // Found an exact case-sensitive match
        }
    }
    return false; // No match found
}

void change_directory(const std::string& inputPath) {
    if (inputPath.empty()) {
        std::cout << "Directory name not specified." << std::endl;
        return;
    }

    // Special case for "cd /" to go to the user's home directory
    if (inputPath == "/") {
        const char* home = getenv("HOME");
        if (home && chdir(home) == 0) {
            std::cout << "Moved to root directory." << std::endl;
            return;
        } else {
            std::cerr << "Failed to move to root directory." << std::endl;
            return;
        }
    }

    fs::path newPath;
    std::vector<std::string> components = split(inputPath, '/');
    bool isAbsolute = inputPath.front() == '/';

    // Start from the root or current path based on the input
    newPath = isAbsolute ? fs::path("/") : fs::current_path();

    for (const auto& component : components) {
        if (isExactComponentMatch(newPath, component)) {
            // Construct the new path incrementally to handle nested directories
            newPath /= component;
        } else {
            std::cout << "Directory does not exist or case mismatch: " << component << std::endl;
            return;
        }
    }

    // Attempt to change to the final path
    if (chdir(newPath.c_str()) == 0) {
        std::cout << "Directory changed to " << newPath << std::endl;
    } else {
        std::cerr << "Failed to change directory to " << newPath << std::endl;
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
