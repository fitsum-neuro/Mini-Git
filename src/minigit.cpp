#include "sha1.hpp" // for hashing
#include <sstream>  // for reading file into string
#include <iomanip>  // for formatting hash output
#include "minigit.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;

std::string file_to_sha1(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return ""; // return empty string if file can't be opened
    }
    
    // read the whole file into a stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();

    // calculate hash
    SHA1 checksum;
    checksum.update(buffer.str());
    return checksum.final();
}

void init() {
    // check if a .minigit repository already exists in the current path
    if (fs::exists(".minigit")) {
        std::cout << "Reinitializing existing MiniGit repository.\n";
    } else {
        std::cout << "Initializing empty MiniGit repository.\n";
    }

    // creates the main .minigit directory and its subdirectories
    fs::create_directory(".minigit");
    fs::create_directory(".minigit/objects");
    fs::create_directory(".minigit/commits");

    // create the HEAD file, which points to the current commit/branch
    // initially, it's empty because there are no commits.
    std::ofstream head_file(".minigit/HEAD");
    head_file.close();

    // create the index file, which acts as the staging area
    std::ofstream index_file(".minigit/index");
    index_file.close();
}


void add(const std::string& filename) {
    if (!fs::exists(".minigit")) {
        std::cerr << "Error: Not a MiniGit repository. Run 'minigit init' first.\n";
        return;
    }

    if (!fs::exists(filename)) {
        std::cerr << "Error: File not found: " << filename << "\n";
        return;
    }

    // calculate the hash of the file content
    std::string hash = file_to_sha1(filename);
    if (hash.empty()) {
        std::cerr << "Error: Could not read file to calculate hash.\n";
        return;
    }

    // the path for the new blob object
    fs::path blob_path = fs::path(".minigit/objects") / hash;

    // copy the file to the objects directory, naming it with its hash
    fs::copy_file(filename, blob_path, fs::copy_options::overwrite_existing);

    // add the file and its hash to the index 
    std::ofstream index_file(".minigit/index", std::ios::app);
    index_file << hash << " " << filename << "\n";
    index_file.close();

    std::cout << "Staged " << filename << "\n";
}