#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <string>

// function declarations for operations
void init();
void add(const std::string& filename);
void commit(const std::string& message);
void log();

#endif // MINIGIT_HPP