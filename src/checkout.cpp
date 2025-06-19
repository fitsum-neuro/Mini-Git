#include "minigit.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>

// Helper: parse commit file, return map<filename, hash>
static std::map<std::string, std::string> parse_commit(const std::string& commit_path) {
    std::ifstream commit_in(commit_path);
    std::map<std::string, std::string> files;
    std::string line;
    // skip parent, date, message
    std::getline(commit_in, line); // parent
    std::getline(commit_in, line); // date
    std::getline(commit_in, line); // message
    while (std::getline(commit_in, line)) {
        if (line.empty()) continue;
        size_t space = line.find(' ');
        if (space != std::string::npos)
            files[line.substr(space + 1)] = line.substr(0, space);
    }
    return files;
}

void checkout(const std::string& name_or_hash) {
    namespace fs = std::filesystem;
    if (!fs::exists(".minigit")) {
        std::cerr << "Error: Not a MiniGit repository.\n";
        return;
    }
    std::string commit_hash;
    // Is it a branch?
    if (fs::exists(".minigit/refs/" + name_or_hash)) {
        std::ifstream branch_in(".minigit/refs/" + name_or_hash);
        std::getline(branch_in, commit_hash);
        branch_in.close();
        // Optionally update CURRENT_BRANCH
        std::ofstream current_branch_out(".minigit/CURRENT_BRANCH");
        current_branch_out << name_or_hash;
        current_branch_out.close();
    } else if (fs::exists(".minigit/commits/" + name_or_hash)) {
        commit_hash = name_or_hash;
        // Optionally clear CURRENT_BRANCH (detached HEAD)
        std::ofstream current_branch_out(".minigit/CURRENT_BRANCH");
        current_branch_out << "";
        current_branch_out.close();
    } else {
        std::cerr << "Branch or commit not found: " << name_or_hash << "\n";
        return;
    }
    std::string commit_path = ".minigit/commits/" + commit_hash;
    if (!fs::exists(commit_path)) {
        std::cerr << "Commit not found: " << commit_hash << "\n";
        return;
    }
    // Parse commit files
    auto files = parse_commit(commit_path);
    // Remove tracked files from working dir (if present and not staged)
    for (const auto& [filename, hash] : files) {
        if (fs::exists(filename)) fs::remove(filename);
    }
    // Restore files from blobs
    for (const auto& [filename, hash] : files) {
        fs::copy_file(".minigit/objects/" + hash, filename, fs::copy_options::overwrite_existing);
    }
    // Update HEAD
    std::ofstream head_out(".minigit/HEAD");
    head_out << commit_hash;
    head_out.close();
    std::cout << "Checked out " << name_or_hash << " (commit " << commit_hash << ")\n";
}
