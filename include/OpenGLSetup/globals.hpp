#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>

extern GLuint textureArray;

extern unsigned int guiVBO;
extern glm::vec4 guiArray[4];

extern unsigned int instanceVBO;
extern glm::ivec4 instanceArray[100000];

extern unsigned int blockIconVBO;
extern glm::ivec4 blockIconArray[256];

extern std::unordered_map<std::string, unsigned short> blockMap;
extern unsigned short blockTextures[65535][6];
extern unsigned short blockCount;

extern std::unordered_map<std::string, unsigned short> textureMap;
extern unsigned short textureCount;