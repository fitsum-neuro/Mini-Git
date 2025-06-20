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
    } else if (command == "branch") {
        if (argc < 3) {
            std::cerr << "Usage: minigit branch <branch-name>\n";
            return 1;
        }
        branch(argv[2]);
    } else if (command == "switch") {
        if (argc < 3) {
            std::cerr << "Usage: minigit switch <branch-name>\n";
            return 1;
        }
        switch_branch(argv[2]);
    } else if (command == "checkout") {
        if (argc < 3) {
            std::cerr << "Usage: minigit checkout <branch-name|commit-hash>\n";
            return 1;
        }
        checkout(argv[2]);
    } else if (command == "merge") {
        if (argc < 3) {
            std::cerr << "Usage: minigit merge <branch-name>\n";
            return 1;
        }
        merge(argv[2]);
    } else {
        std::cerr << "Unknown command: " << command << "\n";
        return 1;
    }

    return 0;
}