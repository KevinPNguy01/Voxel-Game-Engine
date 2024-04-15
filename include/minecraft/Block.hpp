#ifndef BLOCK
#define BLOCK

#include <bitset>
#include <glm/glm.hpp>

using namespace std;

class Block {
public:
    string type;
    glm::ivec3 pos;
    bitset<6> cullCoveredFace;

    Block(int x, int y, int z, string type);
    
    int x() const;
    int y() const;
    int z() const;
};

#endif