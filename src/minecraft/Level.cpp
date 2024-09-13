#include "minecraft/Level.hpp"

Level::Level(int seed) : chunks{}, seedRandomNums{}, noise{}  {
    // Generate random numbers to be used for world generation.
    srand(seed);
    for (int i = 0; i < 500; ++i) {
        seedRandomNums[i] = rand();
    }
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
}

void Level::generateChunk(glm::ivec2 chunkPos) {
    // Create and get new chunk.
    chunks.emplace( chunkPos, Chunk{chunkPos[0], chunkPos[1]} );
    Chunk& c = chunks.at(chunkPos);

    // Generate generate column by column.
    int x1{ chunkPos[0] * 16 }, x2{ x1 + 16 };
    int z1{ chunkPos[1] * 16 }, z2{ z1 + 16 };
    for (int x = x1; x < x2; ++x) {
        for (int z = z1; z < z2; ++z) {
            int y2{ (int)(noise.GetNoise((float)x, (float)z) * 16)  + 16};
            for (int y = 0; y < y2; ++y) {
                string type = "stone";
                if (y2 - y == 1) {
                    type = "grass_block";
                }
                else if (y2 - y < 4) {
                    type = "dirt";
                }
                c.addBlock(Block{ x,y,z, type});
            }
            // Generate a tree with 0.5% chance.
            if (rand() % 200 <= 1) addTree(x, y2, z);
        }
    }

    // Update cull faces for every block in the chunk.
    for (int x = x1; x < x2; ++x) {
        for (int z = z1; z < z2; ++z) {
            int y2{ (int)(noise.GetNoise((float)x, (float)z) * 16) + 16 };
            for (int y = 0; y < y2; ++y) {
                updateCoveredFaces({ x,y,z });
            }
        }
    }
}

void Level::addTree(int x, int y, int z) {
    addBlock(Block{ x,y,z, "oak_log" });
    addBlock(Block{ x,y + 1,z, "oak_log" });
    addBlock(Block{ x,y + 2,z, "oak_log" });
    addBlock(Block{ x,y + 3,z, "oak_log" });
    addBlock(Block{ x,y + 4,z, "oak_log" });

    addBlock(Block{ x + 2,y + 3,z - 1, "oak_leaves" });
    addBlock(Block{ x + 2,y + 3,z, "oak_leaves" });
    addBlock(Block{ x + 2,y + 3,z + 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 3,z - 2, "oak_leaves" });
    addBlock(Block{ x + 1,y + 3,z - 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 3,z, "oak_leaves" });
    addBlock(Block{ x + 1,y + 3,z + 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 3,z + 2, "oak_leaves" });
    addBlock(Block{ x,y + 3,z - 2, "oak_leaves" });
    addBlock(Block{ x,y + 3,z - 1, "oak_leaves" });
    addBlock(Block{ x,y + 3,z + 1, "oak_leaves" });
    addBlock(Block{ x,y + 3,z + 2, "oak_leaves" });
    addBlock(Block{ x - 1,y + 3,z - 2, "oak_leaves" });
    addBlock(Block{ x - 1,y + 3,z - 1, "oak_leaves" });
    addBlock(Block{ x - 1,y + 3,z, "oak_leaves" });
    addBlock(Block{ x - 1,y + 3,z + 1, "oak_leaves" });
    addBlock(Block{ x - 1,y + 3,z + 2, "oak_leaves" });
    addBlock(Block{ x - 2,y + 3,z - 1, "oak_leaves" });
    addBlock(Block{ x - 2,y + 3,z, "oak_leaves" });
    addBlock(Block{ x - 2,y + 3,z + 1, "oak_leaves" });

    addBlock(Block{ x + 2,y + 4,z - 1, "oak_leaves" });
    addBlock(Block{ x + 2,y + 4,z, "oak_leaves" });
    addBlock(Block{ x + 2,y + 4,z + 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 4,z - 2, "oak_leaves" });
    addBlock(Block{ x + 1,y + 4,z - 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 4,z, "oak_leaves" });
    addBlock(Block{ x + 1,y + 4,z + 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 4,z + 2, "oak_leaves" });
    addBlock(Block{ x,y + 4,z - 2, "oak_leaves" });
    addBlock(Block{ x,y + 4,z - 1, "oak_leaves" });
    addBlock(Block{ x,y + 4,z + 1, "oak_leaves" });
    addBlock(Block{ x,y + 4,z + 2, "oak_leaves" });
    addBlock(Block{ x - 1,y + 4,z - 2, "oak_leaves" });
    addBlock(Block{ x - 1,y + 4,z - 1, "oak_leaves" });
    addBlock(Block{ x - 1,y + 4,z, "oak_leaves" });
    addBlock(Block{ x - 1,y + 4,z + 1, "oak_leaves" });
    addBlock(Block{ x - 1,y + 4,z + 2, "oak_leaves" });
    addBlock(Block{ x - 2,y + 4,z - 1, "oak_leaves" });
    addBlock(Block{ x - 2,y + 4,z, "oak_leaves" });
    addBlock(Block{ x - 2,y + 4,z + 1, "oak_leaves" });

    addBlock(Block{ x - 1,y + 5,z - 1, "oak_leaves" });
    addBlock(Block{ x - 1,y + 5,z, "oak_leaves" });
    addBlock(Block{ x - 1,y + 5,z + 1, "oak_leaves" });
    addBlock(Block{ x,y + 5,z - 1, "oak_leaves" });
    addBlock(Block{ x,y + 5,z, "oak_leaves" });
    addBlock(Block{ x,y + 5,z + 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 5,z - 1, "oak_leaves" });
    addBlock(Block{ x + 1,y + 5,z, "oak_leaves" });
    addBlock(Block{ x + 1,y + 5,z + 1, "oak_leaves" });

    addBlock(Block{ x ,y + 6,z, "oak_leaves" });
    addBlock(Block{ x - 1 ,y + 6,z, "oak_leaves" });
    addBlock(Block{ x + 1 ,y + 6,z, "oak_leaves" });
    addBlock(Block{ x ,y + 6,z - 1, "oak_leaves" });
    addBlock(Block{ x ,y + 6,z + 1, "oak_leaves" });
}

void Level::addBlock(Block b) {
    // Generate chunk if it hasn't been generated yet.
	glm::ivec2 chunkPos{ floor(b.x() / 16.f), floor(b.z() / 16.f) };
    if (!chunks.contains(chunkPos)) generateChunk(chunkPos);
	Chunk& c = chunks.at(chunkPos);
	c.addBlock(b);
    updateBlock({b.x(), b.y(), b.z()});
}

void Level::deleteBlock(glm::ivec3 blockPos) {
    // Return if chunk doesn't exist.
    glm::ivec2 chunkPos{ floor(blockPos[0] / 16.f), floor(blockPos[2] / 16.f) };
    if (!chunks.contains(chunkPos)) return;
    chunks.at(chunkPos).deleteBlock(blockPos);
    updateBlock(blockPos);
}

Block* Level::getBlockAt(glm::ivec3 blockPos) {
    // Return nullptr if chunk doesn't exist or block doesn' exist.
    glm::ivec2 chunkPos{ floor(blockPos[0] / 16.0f), floor(blockPos[2] / 16.0f) };
    if (!chunks.contains(chunkPos)) return nullptr;
    Chunk& c = chunks.at(chunkPos);
    if (!c.blocks.contains(blockPos)) return nullptr;
    return &c.blocks.at(blockPos);
}

void Level::updateCoveredFaces(glm::ivec3 blockPos) {
    Block* b = getBlockAt(blockPos);
    if (b == nullptr) return;
    // Check if each adjacent face is covered.
    int d[][3] = { {0,0,-1}, {0,0,1}, {0,-1,0}, {0,1,0}, {-1,0,0}, {1,0,0} };
    for (int i = 0; i < 6; ++i) {
        b->cullCoveredFace.set(i, getBlockAt({ blockPos[0] + d[i][0], blockPos[1] + d[i][1], blockPos[2] + d[i][2] }));
    }
}

void Level::updateBlock(glm::ivec3 blockPos) {
    // Update faces at this position, and then adjacent positions.
    updateCoveredFaces(blockPos);
    int d[][3] = { {0,0,-1}, {0,0,1}, {0,-1,0}, {0,1,0}, {-1,0,0}, {1,0,0} };
    for (int i = 0; i < 6; ++i) {
        updateCoveredFaces({ blockPos[0] + d[i][0], blockPos[1] + d[i][1], blockPos[2] + d[i][2] });
    }
}

size_t ChunkHashEquals::operator()(const glm::ivec2& p) const {
    std::hash<int> hasher;
    size_t hashValue = 0;
    hashValue ^= hasher(p[0]) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    hashValue ^= hasher(p[1]) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    return hashValue;
}

bool ChunkHashEquals::operator()(const glm::ivec2& a, const glm::ivec2& b) const {
    return a[0] == b[0] && a[1] == b[1];
}