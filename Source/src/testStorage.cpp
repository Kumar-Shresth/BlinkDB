#include "storageEngine.h"
#include <iostream>

int main() {
    StorageEngine db;

    db.set("mykey", "myvalue");
    std::cout << "GET mykey: " << db.get("mykey") << std::endl;  // Expected: myvalue

    db.del("mykey");
    std::cout << "GET mykey: " << db.get("mykey") << std::endl;  // Expected: NULL

    return 0;
}
