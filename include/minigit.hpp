#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <string>

// function declarations for operations
void init();
void add(const std::string& filename);
void commit(const std::string& message);
void log();
void branch(const std::string& name);
void switch_branch(const std::string& name);
void checkout(const std::string& name_or_hash);
void merge(const std::string& branch_name);
void diff(const std::string& commit_hash1, const std::string& commit_hash2); 

#endif // MINIGIT_HPP