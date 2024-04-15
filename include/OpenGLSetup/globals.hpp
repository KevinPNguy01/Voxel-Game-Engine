#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/glad.h>

extern unsigned int instanceVBO;
extern glm::ivec4 instanceArray[100000];

extern std::unordered_map<std::string, unsigned short> blockMap;
extern short blockTextures[6][65535];
extern unsigned short blockCount;

extern std::unordered_map<std::string, unsigned short> textureMap;
extern unsigned short textureCount;