#include "storageEngine.h"
#include <iostream>
#include <sstream>

void startREPL() {
    StorageEngine db;
    std::string line, command, key, value;

    while (true) {
        std::cout << "User> ";
        std::getline(std::cin, line);

        std::istringstream iss(line);
        iss >> command >> key;

        if (command == "SET") {
            iss.ignore(); // Ignore the space before value
            std::getline(iss, value);
            db.set(key, value);
        } else if (command == "GET") {
            std::cout << db.get(key) << std::endl;
        } else if (command == "DEL") {
            db.del(key);
        } else if (command == "EXIT") {
            break;
        } else {
            std::cout << "Invalid command!" << std::endl;
        }
    }
}

int main() {
    startREPL();
    return 0;
}
