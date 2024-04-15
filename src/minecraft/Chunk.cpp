#include "minecraft/Chunk.hpp"

Chunk::Chunk(int x, int z) : blocks{}, x{x}, z{z} {}

void Chunk::addBlock(Block b) {
    blocks.insert({{b.x(), b.y(), b.z()}, b});
}

void Chunk::deleteBlock(glm::ivec3 blockPos) {
    blocks.erase(blockPos);
}

size_t BlockHashEquals::operator()(const glm::ivec3& p) const {
    std::hash<int> hasher;
    size_t hashValue = 0;
    hashValue ^= hasher(p[0]) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    hashValue ^= hasher(p[1]) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    hashValue ^= hasher(p[2]) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    return hashValue;
}

bool BlockHashEquals::operator()(const glm::ivec3& a, const glm::ivec3& b) const {
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}