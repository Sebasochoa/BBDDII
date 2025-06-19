#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <string>
#include <unordered_map>
#include <list>

struct BufferFrame
{
    int id;
    std::string path;
    std::string data;
    bool dirty = false;
    bool pinned = false; 
    bool write = false; 
};

class BufferManager
{
public:
    enum Policy
    {
        LRU,
        FIFO
    };

    BufferManager(size_t numFrames = 3, Policy policy = LRU);
    ~BufferManager();   
    std::string &readBlock(int blockId, const std::string &path, bool write = false, bool pinned = false);
    void unpin(int blockId);
    void printPageTable() const;
    void markDirty(int blockId);
    void writeBlock(int blockId);
    void flushAll();
private:
    size_t capacity;
    Policy policy;
    std::unordered_map<int, BufferFrame> frames;
    std::list<int> order;
    std::unordered_map<int, std::list<int>::iterator> positions;
    void touch(int blockId);
    void evictIfNeeded();
};

#endif