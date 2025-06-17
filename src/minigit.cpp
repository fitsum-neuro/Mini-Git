#include "minigit.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;

void init() {
    // check if a .minigit repository already exists in the current path
    if (fs::exists(".minigit")) {
        std::cout << "Reinitializing existing MiniGit repository.\n";
    } else {
        std::cout << "Initializing empty MiniGit repository.\n";
    }

    // Create the main .minigit directory and its subdirectories
    fs::create_directory(".minigit");
    fs::create_directory(".minigit/objects");
    fs::create_directory(".minigit/commits");

    // Create the HEAD file, which points to the current commit/branch
    // Initially, it's empty because there are no commits.
    std::ofstream head_file(".minigit/HEAD");
    head_file.close();

    // Create the index file, which acts as the staging area
    std::ofstream index_file(".minigit/index");
    index_file.close();
}