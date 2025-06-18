#include "BufferManager.h"
#include <iostream>
#include <fstream>

BufferManager::BufferManager(size_t numFrames, Policy pol)
    : capacity(numFrames), policy(pol) {}

BufferManager::~BufferManager() {
    flushAll();
}

std::string &BufferManager::readBlock(int blockId, const std::string &path) {
    auto it = frames.find(blockId);
    if (it != frames.end()) {
        touch(blockId);
        return it->second.data;
    }
    evictIfNeeded();
    BufferFrame frame;
    frame.id = blockId;
    frame.path = path;
    std::ifstream in(path, std::ios::binary);
    if (in) {
        frame.data.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }
    in.close();
    frames[blockId] = frame;
    order.push_back(blockId);
    positions[blockId] = std::prev(order.end());
    return frames[blockId].data;
}

void BufferManager::markDirty(int blockId) {
    auto it = frames.find(blockId);
    if (it != frames.end()) {
        it->second.dirty = true;
        touch(blockId);
    }
}

void BufferManager::writeBlock(int blockId) {
    auto it = frames.find(blockId);
    if (it != frames.end() && it->second.dirty) {
        std::ofstream out(it->second.path, std::ios::binary | std::ios::trunc);
        if (out) {
            out.write(it->second.data.data(), it->second.data.size());
            out.close();
            it->second.dirty = false;
        }
    }
}

void BufferManager::flushAll() {
    for (auto &kv : frames) {
        if (kv.second.dirty) {
            std::ofstream out(kv.second.path, std::ios::binary | std::ios::trunc);
            if (out) {
                out.write(kv.second.data.data(), kv.second.data.size());
                out.close();
                kv.second.dirty = false;
            }
        }
    }
}

void BufferManager::touch(int blockId) {
    if (policy == LRU) {
        auto posIt = positions.find(blockId);
        if (posIt != positions.end()) {
            order.erase(posIt->second);
            order.push_back(blockId);
            positions[blockId] = std::prev(order.end());
        }
    }
}

void BufferManager::evictIfNeeded() {
    if (frames.size() < capacity)
        return;
    int evictId = order.front();
    order.pop_front();
    positions.erase(evictId);
    writeBlock(evictId);
    frames.erase(evictId);
}