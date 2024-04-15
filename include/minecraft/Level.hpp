#ifndef LEVEL
#define LEVEL

#include "Chunk.hpp"
#include "FastNoiseLite.h"

struct ChunkHashEquals {
    size_t operator()(const glm::ivec2& p) const;
    bool operator()(const glm::ivec2& a, const glm::ivec2& b) const;
};

class Level {
    FastNoiseLite noise;
    float seedRandomNums[500];
public:
    unordered_map<glm::ivec2, Chunk, ChunkHashEquals, ChunkHashEquals> chunks;

    Level(int seed);

    void addBlock(Block b);
    void deleteBlock(glm::ivec3 blockPos);
    Block* getBlockAt(glm::ivec3 blockPos);
    void updateCoveredFaces(glm::ivec3 blockPos);
    void updateBlock(glm::ivec3 blockPos);

    void generateChunk(glm::ivec2 chunkPos);
    void addTree(int x, int y, int z);
};

#endif