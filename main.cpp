#include "minigit.hpp"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: minigit <command> [args]\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "init") {
        init();
    } else if (command == "add") {
        if (argc < 3) {
            std::cerr << "Usage: minigit add <filename>\n";
            return 1;
        }
        add(argv[2]);
    }
    else if (command == "commit") {
        if (argc < 4 || std::string(argv[2]) != "-m") {
            std::cerr << "Usage: minigit commit -m \"<message>\"\n";
            return 1;
        }
        commit(argv[3]);
    } else if (command == "log") {
        log();
    } else {
        std::cerr << "Unknown command: " << command << "\n";
        return 1;
    }

    return 0;
}