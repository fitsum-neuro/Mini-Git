#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <string>

// function declarations for operations
void init();
void add(const std::string& filename);
void commit(const std::string& message);

#endif // MINIGIT_HPP