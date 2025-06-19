#include "minigit.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <map>
#include <string>

namespace fs = std::filesystem;

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

// Helper: get parent commit hash
static std::string get_parent(const std::string& commit_path) {
    std::ifstream commit_in(commit_path);
    std::string parent_line;
    std::getline(commit_in, parent_line);
    size_t pos = parent_line.find(": ");
    if (pos != std::string::npos)
        return parent_line.substr(pos + 2);
    return "";
}

// Helper: get all ancestors of a commit
static std::set<std::string> get_ancestors(const std::string& commit_hash) {
    std::set<std::string> ancestors;
    std::string current = commit_hash;
    while (!current.empty() && current != "null") {
        ancestors.insert(current);
        std::string commit_path = ".minigit/commits/" + current;
        if (!fs::exists(commit_path)) break;
        current = get_parent(commit_path);
    }
    return ancestors;
}

void merge(const std::string& branch_name) {
    // Find HEAD and target branch commit hashes
    std::ifstream head_in(".minigit/HEAD");
    std::string head_hash;
    std::getline(head_in, head_hash);
    head_in.close();
    std::ifstream branch_in(".minigit/refs/" + branch_name);
    if (!branch_in) {
        std::cerr << "Branch not found: " << branch_name << "\n";
        return;
    }
    std::string branch_hash;
    std::getline(branch_in, branch_hash);
    branch_in.close();
    if (branch_hash == head_hash) {
        std::cout << "Already up to date.\n";
        return;
    }
    // Find LCA
    auto head_ancestors = get_ancestors(head_hash);
    std::string lca = branch_hash;
    while (!lca.empty() && lca != "null" && head_ancestors.find(lca) == head_ancestors.end()) {
        lca = get_parent(".minigit/commits/" + lca);
    }
    if (lca.empty() || lca == "null") {
        std::cerr << "No common ancestor found.\n";
        return;
    }
    // Parse file trees
    auto base = parse_commit(".minigit/commits/" + lca);
    auto head = parse_commit(".minigit/commits/" + head_hash);
    auto other = parse_commit(".minigit/commits/" + branch_hash);
    std::set<std::string> all_files;
    for (const auto& [f, _] : base) all_files.insert(f);
    for (const auto& [f, _] : head) all_files.insert(f);
    for (const auto& [f, _] : other) all_files.insert(f);
    bool conflict = false;
    for (const auto& file : all_files) {
        std::string base_hash = base.count(file) ? base[file] : "";
        std::string head_hash_f = head.count(file) ? head[file] : "";
        std::string other_hash = other.count(file) ? other[file] : "";
        // Three-way merge logic
        if (head_hash_f == other_hash || other_hash == base_hash) {
            // Use HEAD version
            if (!head_hash_f.empty())
                fs::copy_file(".minigit/objects/" + head_hash_f, file, fs::copy_options::overwrite_existing);
            else if (fs::exists(file)) fs::remove(file);
        } else if (head_hash_f == base_hash) {
            // Use branch version
            if (!other_hash.empty())
                fs::copy_file(".minigit/objects/" + other_hash, file, fs::copy_options::overwrite_existing);
            else if (fs::exists(file)) fs::remove(file);
        } else if (base_hash != head_hash_f && base_hash != other_hash && head_hash_f != other_hash) {
            // Conflict
            std::ifstream head_file(".minigit/objects/" + head_hash_f);
            std::ifstream other_file(".minigit/objects/" + other_hash);
            std::ofstream out(file);
            out << "<<<<<<< HEAD\n";
            out << (head_file ? std::string((std::istreambuf_iterator<char>(head_file)), std::istreambuf_iterator<char>()) : "") << "\n";
            out << "=======\n";
            out << (other_file ? std::string((std::istreambuf_iterator<char>(other_file)), std::istreambuf_iterator<char>()) : "") << "\n";
            out << ">>>>>>> " << branch_name << "\n";
            std::cerr << "CONFLICT: both modified " << file << "\n";
            conflict = true;
        }
    }
    if (conflict) {
        std::cout << "Merge completed with conflicts. Please resolve them and commit.\n";
    } else {
        std::cout << "Merge completed successfully. You may now commit the result.\n";
    }
}
