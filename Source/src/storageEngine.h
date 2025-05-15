#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <iostream>
#include <fstream>

class StorageEngine {
private:
    std::unordered_map<std::string, std::string> data;
    std::unordered_set<std::string> deletedKeys;
    std::list<std::string> usageList;
    std::unordered_map<std::string, std::list<std::string>::iterator> usageMap;
    
    // LRU Cache
    std::list<std::string> cacheList;
    std::unordered_map<std::string, std::pair<std::list<std::string>::iterator, std::string>> cacheMap;
    size_t cacheSize = 3; // Adjust based on memory constraints
    
    // File indexing for efficient retrieval
    std::unordered_map<std::string, std::streampos> fileIndex;
    
    size_t maxSize = 3;
    
    void evictIfNeeded();
    void persistEvictedKey(const std::string& key, const std::string& value);
    std::string retrieveEvictedKey(const std::string& key);
    void updateFileIndex();

public:
    StorageEngine();
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void del(const std::string& key);
};

#endif // STORAGE_ENGINE_H
