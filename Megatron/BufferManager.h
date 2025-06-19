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
    bool pinned = false; // if true the page cannot be evicted
    bool write = false;  // true if the page was requested for writing
};

class BufferManager
{
public:
    enum Policy
    {
        LRU,
        FIFO
    };

    BufferManager(size_t numFrames = 5, Policy policy = LRU);
    ~BufferManager();

    // Load block into memory and return reference to its data. If write is
    // true the caller intends to modify the page. pinned controls eviction.
    std::string &readBlock(int blockId, const std::string &path,
                           bool write = false, bool pinned = false);

    // Unpin a page so it can be evicted
    void unpin(int blockId);

    // Print the current page table following LRU order
    void printPageTable() const;

    // Mark a block as modified
    void markDirty(int blockId);

    // Write a block back to disk if dirty
    void writeBlock(int blockId);

    // Flush all dirty blocks to disk
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