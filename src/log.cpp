#include "minigit.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

void log() {
    namespace fs = std::filesystem;
    if (!fs::exists(".minigit")) {
        std::cerr << "Error: Not a MiniGit repository.\n";
        return;
    }
    std::ifstream head_in(".minigit/HEAD");
    std::string current_hash;
    std::getline(head_in, current_hash);
    head_in.close();
    if (current_hash.empty()) {
        std::cout << "No commits yet.\n";
        return;
    }
    while (!current_hash.empty() && current_hash != "null") {
        std::string commit_path = ".minigit/commits/" + current_hash;
        std::ifstream commit_in(commit_path);
        if (!commit_in) {
            std::cerr << "Corrupted commit history at " << current_hash << "\n";
            break;
        }
        std::string parent, date, message;
        std::getline(commit_in, parent); // parent: ...
        std::getline(commit_in, date);   // date: ...
        std::getline(commit_in, message); // message: ...
        std::cout << "Commit: " << current_hash << "\n";
        std::cout << date << "\n";
        std::cout << message << "\n";
        std::cout << "--------------------------\n";
        // extract parent hash
        size_t pos = parent.find(": ");
        if (pos != std::string::npos) {
            current_hash = parent.substr(pos + 2);
        } else {
            break;
        }
        if (current_hash == "null") break;
    }
}
