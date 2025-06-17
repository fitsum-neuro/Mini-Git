#include <chrono> //for timestamp
#include <ctime> // for timestamp
#include <map>   //to hold index data
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

void commit(const std::string& message) {
    if (!fs::exists(".minigit")) {
        std::cerr << "Error: Not a MiniGit repository.\n";
        return;
    }

    //  read the staging area (index)
    std::map<std::string, std::string> staged_files;
    std::ifstream index_in(".minigit/index");
    std::string hash, filename;
    while (index_in >> hash >> filename) {
        staged_files[filename] = hash;
    }
    index_in.close();

    if (staged_files.empty()) {
        std::cout << "Nothing to commit, working tree clean.\n";
        return;
    }

    //  get the parent commit hash from HEAD
    std::ifstream head_in(".minigit/HEAD");
    std::string parent_commit;
    std::getline(head_in, parent_commit);
    head_in.close();

    // construct the commit content string
    std::stringstream commit_content;
    commit_content << "parent: " << (parent_commit.empty() ? "null" : parent_commit) << "\n";
    
    // add a timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    commit_content << "date: " << std::ctime(&now_time); // ctime adds a newline
    
    commit_content << "message: " << message << "\n\n";

    // add the staged files to the commit content
    for (const auto& pair : staged_files) {
        commit_content << pair.second << " " << pair.first << "\n";
    }

    // calculate the hash of the commit content itself
    SHA1 commit_checksum;
    commit_checksum.update(commit_content.str());
    std::string commit_hash = commit_checksum.final();

    // write the commit object to a file named with its hash
    std::ofstream commit_file(".minigit/commits/" + commit_hash);
    commit_file << commit_content.str();
    commit_file.close();

    // update HEAD to point to this new commit
    std::ofstream head_out(".minigit/HEAD");
    head_out << commit_hash;
    head_out.close();

    // clear the index (staging area) since changes are now committed
    std::ofstream index_clear(".minigit/index", std::ios::trunc);
    index_clear.close();

    std::cout << "Committed with ID " << commit_hash << "\n";
}