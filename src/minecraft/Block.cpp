#include "minecraft/Block.hpp"

using namespace std;

Block::Block(int x, int y, int z, string type) : pos{ x,y,z }, type{ type }, cullCoveredFace{63} {}

int Block::x() const { return pos[0]; }
int Block::y() const { return pos[1]; }
int Block::z() const { return pos[2]; }