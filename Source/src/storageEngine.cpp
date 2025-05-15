#include "storageEngine.h"

StorageEngine::StorageEngine() {
    std::ofstream file("evicted_data.txt", std::ios::trunc);
    file.close();
    updateFileIndex(); // Build file index on startup
}

void StorageEngine::evictIfNeeded() {
    if (data.size() >= maxSize && !usageList.empty()) {
        std::string lruKey = usageList.back();
        std::string lruValue = data[lruKey];

        persistEvictedKey(lruKey, lruValue);
        
        usageList.pop_back();
        usageMap.erase(lruKey);
        data.erase(lruKey);
        
        std::cout << "Evicted: " << lruKey << std::endl;
    }
}

void StorageEngine::persistEvictedKey(const std::string& key, const std::string& value) {
    std::ofstream file("evicted_data.txt", std::ios::app);
    if (file.is_open()) {
        std::streampos pos = file.tellp(); // Store file position
        file << key << " " << value << "\n";
        fileIndex[key] = pos;
        file.close();
    }
}

// Optimized retrieval using file index
std::string StorageEngine::retrieveEvictedKey(const std::string& key) {
    if (fileIndex.find(key) == fileIndex.end()) return "NULL";

    std::ifstream file("evicted_data.txt");
    if (!file.is_open()) return "NULL";

    file.seekg(fileIndex[key]); // Jump to key's position
    std::string k, v;
    file >> k;
    file.ignore();
    std::getline(file, v);

    file.close();
    return (k == key) ? v : "NULL";
}

void StorageEngine::updateFileIndex() {
    std::ifstream file("evicted_data.txt");
    if (!file.is_open()) return;

    std::string key, value;
    std::streampos pos;
    while (file) {
        pos = file.tellg();
        file >> key;
        if (!file) break;
        file.ignore();
        std::getline(file, value);
        fileIndex[key] = pos;
    }
    
    file.close();
}

void StorageEngine::set(const std::string& key, const std::string& value) {
    if (usageMap.count(key)) {
        usageList.erase(usageMap[key]);
    }

    evictIfNeeded();

    usageList.push_front(key);
    usageMap[key] = usageList.begin();
    data[key] = value;

    // Add to cache
    if (cacheMap.size() >= cacheSize) {
        std::string lruCacheKey = cacheList.back();
        cacheList.pop_back();
        cacheMap.erase(lruCacheKey);
    }

    cacheList.push_front(key);
    cacheMap[key] = {cacheList.begin(), value};
}

std::string StorageEngine::get(const std::string& key) {
    if (deletedKeys.count(key)) return "NULL";  

    // Check cache first
    if (cacheMap.count(key)) {
        // Move accessed item to front
        cacheList.erase(cacheMap[key].first);
        cacheList.push_front(key);
        cacheMap[key].first = cacheList.begin();
        return cacheMap[key].second;
    }

    // Check in-memory data
    if (data.count(key)) return data[key];

    // Retrieve from evicted file
    std::string value = retrieveEvictedKey(key);
    if (value != "NULL" && !deletedKeys.count(key)) {
        std::cout << "Restoring evicted key: " << key << std::endl;
        data[key] = value;

        usageList.push_front(key);
        usageMap[key] = usageList.begin();

        // Add to cache
        if (cacheMap.size() >= cacheSize) {
            std::string lruCacheKey = cacheList.back();
            cacheList.pop_back();
            cacheMap.erase(lruCacheKey);
        }

        cacheList.push_front(key);
        cacheMap[key] = {cacheList.begin(), value};

        return value;
    }

    return "NULL";
}

void StorageEngine::del(const std::string& key) {
    if (deletedKeys.count(key)) return;

    if (data.erase(key) > 0) {
        deletedKeys.insert(key);
        if (usageMap.count(key)) {
            usageList.erase(usageMap[key]);
            usageMap.erase(key);
        }
    } else if (retrieveEvictedKey(key) != "NULL") {
        deletedKeys.insert(key);
    } else {
        std::cout << "Does not exist." << std::endl;
    }

    // Remove from cache
    if (cacheMap.count(key)) {
        cacheList.erase(cacheMap[key].first);
        cacheMap.erase(key);
    }
}
