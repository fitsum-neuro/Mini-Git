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
#include <vector> 

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

// helper function to load the contents of a commit into a map
// the map will store: <filename, content_hash>
std::map<std::string, std::string> load_commit_data(const std::string& commit_hash) {
    std::map<std::string, std::string> commit_data;
    fs::path commit_path = fs::path(".minigit/commits") / commit_hash;

    if (!fs::exists(commit_path)) {
        return commit_data; // return empty map if commit not found
    }

    std::ifstream commit_file(commit_path);
    std::string line;
    bool content_started = false;

    while (std::getline(commit_file, line)) {
        if (line.empty()) {
            content_started = true; // the file content listing starts after the first blank line
            continue;
        }
        if (content_started) {
            std::stringstream ss(line);
            std::string content_hash, filename;
            ss >> content_hash >> filename;
            if (!content_hash.empty() && !filename.empty()) {
                commit_data[filename] = content_hash;
            }
        }
    }
    return commit_data;
}

// helper function to read a blob object into a vector of strings
std::vector<std::string> read_blob_lines(const std::string& content_hash) {
    std::vector<std::string> lines;
    fs::path blob_path = fs::path(".minigit/objects") / content_hash;

    if (!fs::exists(blob_path)) {
        return lines; // return empty vector if blob not found
    }

    std::ifstream blob_file(blob_path);
    std::string line;
    while (std::getline(blob_file, line)) {
        lines.push_back(line);
    }
    return lines;
}


void diff(const std::string& commit_hash1, const std::string& commit_hash2) {
    if (!fs::exists(".minigit")) {
        std::cerr << "Error: Not a MiniGit repository.\n";
        return;
    }

    // 1 loads the data for both commits
    std::map<std::string, std::string> commit1_data = load_commit_data(commit_hash1);
    std::map<std::string, std::string> commit2_data = load_commit_data(commit_hash2);

    if (commit1_data.empty()) {
        std::cerr << "Error: Could not find commit " << commit_hash1 << "\n";
        return;
    }
    if (commit2_data.empty()) {
        std::cerr << "Error: Could not find commit " << commit_hash2 << "\n";
        return;
    }

    // 2 compares the file lists
    for (const auto& pair2 : commit2_data) {
        const std::string& filename = pair2.first;
        const std::string& hash2 = pair2.second;

        auto it1 = commit1_data.find(filename);

        if (it1 == commit1_data.end()) {
            // file exists in commit2 but not in commit1
            std::cout << "--- /dev/null\n";
            std::cout << "+++ " << filename << "\n";
            std::vector<std::string> lines = read_blob_lines(hash2);
            for (const auto& line : lines) {
                std::cout << "+ " << line << "\n";
            }
            std::cout << "\n";
        } else {
            // file exists in both commits, check if modified
            const std::string& hash1 = it1->second;
            if (hash1 != hash2) {
                // hashes are different 
                std::cout << "--- " << filename << "\n";
                std::cout << "+++ " << filename << "\n";
                std::vector<std::string> lines1 = read_blob_lines(hash1);
                std::vector<std::string> lines2 = read_blob_lines(hash2);

                // simple diff: show all old lines with '-', all new lines with '+'
                for (const auto& line : lines1) {
                    std::cout << "- " << line << "\n";
                }
                for (const auto& line : lines2) {
                    std::cout << "+ " << line << "\n";
                }
                std::cout << "\n";
            }
            // erase from commit1_data so we can find deleted files later
            commit1_data.erase(it1);
        }
    }

    // 3 any files left in commit1_data were deleted in commit2
    for (const auto& pair1 : commit1_data) {
        const std::string& filename = pair1.first;
        const std::string& hash1 = pair1.second;
        std::cout << "--- " << filename << "\n";
        std::cout << "+++ /dev/null\n";
        std::vector<std::string> lines = read_blob_lines(hash1);
        for (const auto& line : lines) {
            std::cout << "- " << line << "\n";
        }
        std::cout << "\n";
    }
}