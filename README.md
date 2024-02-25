# Encrypted Secure File System

## Overview

The Encrypted Secure File System is a middleware solution designed to enhance file security within a host file system. It prioritizes confidentiality and security, ensuring that users cannot extract any information about the stored data or associated users. This system supports multiple clients/users under a unified storage approach, maximizing resource utilization without requiring separate storage allocations for individual users.

## Getting Started

### Prerequisites

- CMake (version 3.10 or higher recommended)
- C++ Compiler (GCC or Clang)
- Linux/Unix environment

### Installation

1. Clone the repository or download the source files.
2. Navigate to the project directory where the `CMakeLists.txt` file is located.
3. Create a build directory and navigate into it
4. Run CMake to configure the project and generate a makefile
5. Compile the project
6. After compilation, the executable `cmpt785_g5_secure_filesystem` will be created in the build directory.

### Running the File Server

Execute the command below to start the file server:

./cmpt785_g5_secure_filesystem admin

This command initializes the encrypted file system, creating a new directory called `filesystem` for storage, and an `admin_keyfile` for the admin user. Public key files are stored in a `public_keys` directory outside the `filesystem`.

### Usage

Upon successful login, users are presented with a shell to interact with the file system. The shell supports the following commands:

- `cd <directory>`: Change the current working directory.
- `pwd`: Print the current working directory.
- `ls`: List files and directories in the current directory.
- `cat <filename>`: Display the decrypted contents of a file.
- `share <filename> <username>`: Share a file with another user, making it available in their `/shared` directory.
- `mkdir <directory_name>`: Create a new directory.
- `mkfile <filename> <contents>`: Create a new file with the specified contents.
- `exit`: Terminate the program.

Admin users have additional capabilities, such as reading the entire file system and adding new users with the `adduser <username>` command.

## Features

- **Secure File Storage**: Files are encrypted and stored securely, ensuring data confidentiality.
- **Multi-User Support**: Supports one admin and multiple users, with personalized access and shared spaces.
- **File Sharing**: Users can share files securely with read-only access.
- **Command-Line Interface**: A user-friendly CLI for navigating and managing the file system.

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests for any enhancements, bug fixes, or documentation improvements.

