#include "BufferManager.h"
#include <iostream>
#include <fstream>

BufferManager::BufferManager(size_t numFrames, Policy pol) : capacity(numFrames), policy(pol)
{
    clockHand = order.end();
}

BufferManager::~BufferManager()
{
    flushAll();
}

std::string &BufferManager::readBlock(int blockId, const std::string &path, bool write, bool pinned)
{
    auto it = frames.find(blockId);
    if (it != frames.end())
    {
        touch(blockId);
        it->second.pinned = pinned;
        it->second.write = write;
        it->second.reference = true;
        it->second.processes++;
        return it->second.data;
    }
    evictIfNeeded();
    BufferFrame frame;
    frame.id = blockId;
    frame.path = path;
    frame.write = write;
    frame.pinned = pinned;
    frame.reference = true;
    frame.processes = 1;
    std::ifstream in(path, std::ios::binary);
    if (in)
    {
        frame.data.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }
    in.close();
    frames[blockId] = frame;
    order.push_back(blockId);
    positions[blockId] = std::prev(order.end());
    if (policy == CLOCK && order.size() == 1)
        clockHand = order.begin();
    return frames[blockId].data;
}

void BufferManager::markDirty(int blockId)
{
    auto it = frames.find(blockId);
    if (it != frames.end())
    {
        it->second.dirty = true;
        it->second.write = true;
        touch(blockId);
    }
}

void BufferManager::writeBlock(int blockId)
{
    auto it = frames.find(blockId);
    if (it != frames.end() && it->second.dirty)
    {
        std::ofstream out(it->second.path, std::ios::binary | std::ios::trunc);
        if (out)
        {
            out.write(it->second.data.data(), it->second.data.size());
            out.close();
            it->second.dirty = false;
        }
    }
}

void BufferManager::flushAll()
{
    for (auto &kv : frames)
    {
        if (kv.second.dirty)
        {
            std::ofstream out(kv.second.path, std::ios::binary | std::ios::trunc);
            if (out)
            {
                out.write(kv.second.data.data(), kv.second.data.size());
                out.close();
                kv.second.dirty = false;
            }
        }
    }
}

void BufferManager::unpin(int blockId)
{
    auto it = frames.find(blockId);
    if (it != frames.end())
    {
        it->second.pinned = false;
        if (it->second.processes > 0)
            --it->second.processes;
    }
}

void BufferManager::release(int blockId)
{
    auto it = frames.find(blockId);
    if (it != frames.end() && it->second.processes > 0)
    {
        --it->second.processes;
    }
}

void BufferManager::printPageTable() const
{
    std::string polStr = "";
    switch (policy)
    {
    case LRU:
        polStr = "LRU";
        break;
    case FIFO:
        polStr = "FIFO";
        break;
    case CLOCK:
        polStr = "CLOCK";
        break;
    }
    std::cout << "Page Table (" << polStr << ")\n";
    std::cout << "ID\tDirty\tPinned\tWrite\tProc\tPath\n";
    for (int id : order)
    {
        auto it = frames.find(id);
        if (it != frames.end())
        {
            const BufferFrame &f = it->second;
            std::cout << f.id << "\t" << (f.dirty ? "Y" : "N") << "\t"
                      << (f.pinned ? "Y" : "N") << "\t"
                      << (f.write ? "W" : "R") << "\t"
                      << f.processes << "\t" << f.path << "\n";
        }
    }
}

void BufferManager::touch(int blockId)
{
    if (policy == LRU)
    {
        auto posIt = positions.find(blockId);
        if (posIt != positions.end())
        {
            order.erase(posIt->second);
            order.push_back(blockId);
            positions[blockId] = std::prev(order.end());
        }
    }
    else if (policy == CLOCK)
    {
        auto it = frames.find(blockId);
        if (it != frames.end())
            it->second.reference = true;
    }
}

void BufferManager::evictIfNeeded()
{
    if (frames.size() < capacity)
        return;
    if (policy == CLOCK)
    {
        if (clockHand == order.end())
            clockHand = order.begin();
        while (!order.empty())
        {
            if (clockHand == order.end())
                clockHand = order.begin();
            int id = *clockHand;
            auto fIt = frames.find(id);
            if (fIt != frames.end())
            {
                BufferFrame &frame = fIt->second;
                if (frame.pinned)
                {
                    ++clockHand;
                    continue;
                }
                if (frame.reference)
                {
                    frame.reference = false;
                    ++clockHand;
                }
                else
                {
                    int evictId = id;
                    auto eraseIt = clockHand;
                    ++clockHand;
                    order.erase(eraseIt);
                    positions.erase(evictId);
                    writeBlock(evictId);
                    frames.erase(evictId);
                    return;
                }
            }
            else
            {
                ++clockHand;
            }
        }
    }
    else
    {
        for (auto it = order.begin(); it != order.end(); ++it)
        {
            int id = *it;
            auto fIt = frames.find(id);
            if (fIt != frames.end() && !fIt->second.pinned)
            {
                int evictId = id;
                order.erase(it);
                positions.erase(evictId);
                writeBlock(evictId);
                frames.erase(evictId);
                return;
            }
        }
    }
}