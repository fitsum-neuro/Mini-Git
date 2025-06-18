#include "minigit.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

void branch(const std::string& name) {
    namespace fs = std::filesystem;
    if (!fs::exists(".minigit")) {
        std::cerr << "Error: Not a MiniGit repository.\n";
        return;
    }
    fs::create_directories(".minigit/refs");
    std::ifstream head_in(".minigit/HEAD");
    std::string head_hash;
    std::getline(head_in, head_hash);
    head_in.close();
    if (head_hash.empty()) {
        std::cerr << "No commits to branch from.\n";
        return;
    }
    std::ofstream branch_out(".minigit/refs/" + name);
    branch_out << head_hash;
    branch_out.close();
    // Optionally, update CURRENT_BRANCH
    std::ofstream current_branch_out(".minigit/CURRENT_BRANCH");
    current_branch_out << name;
    current_branch_out.close();
    std::cout << "Created branch '" << name << "' at commit " << head_hash << "\n";
}

void switch_branch(const std::string& name) {
    namespace fs = std::filesystem;
    if (!fs::exists(".minigit/refs/" + name)) {
        std::cerr << "Branch '" << name << "' does not exist.\n";
        return;
    }
    std::ifstream branch_in(".minigit/refs/" + name);
    std::string branch_hash;
    std::getline(branch_in, branch_hash);
    branch_in.close();
    std::ofstream head_out(".minigit/HEAD");
    head_out << branch_hash;
    head_out.close();
    // Optionally, update CURRENT_BRANCH
    std::ofstream current_branch_out(".minigit/CURRENT_BRANCH");
    current_branch_out << name;
    current_branch_out.close();
    std::cout << "Switched to branch '" << name << "' at commit " << branch_hash << "\n";
}
