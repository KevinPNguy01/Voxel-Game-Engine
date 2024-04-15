#ifndef CHUNK
#define CHUNK

#include <unordered_map>
#include <glm/glm.hpp>
#include "Block.hpp"

using namespace std;

struct BlockHashEquals {
    size_t operator()(const glm::ivec3& p) const;
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const;
};

class Chunk {
public:
    int x, z;
    unordered_map<glm::ivec3, Block, BlockHashEquals, BlockHashEquals> blocks;

    Chunk(int x, int z);

    void addBlock(Block b);
    void deleteBlock(glm::ivec3 blockPos);
};

#endif