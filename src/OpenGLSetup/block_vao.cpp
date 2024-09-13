#include "OpenGLSetup/block_vao.hpp"

GLuint createBlockVAO() {
    GLuint vertices[] = { 0,1,2,3 };

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLuint), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 100000 * sizeof(int) * 4, nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(int), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glGenTextures(1, &textureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, 16, 16, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    loadModels("rsrc/models/block");

    return VAO;
}

void loadModels(std::string folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            loadModel(entry);
        }
    }
}

void loadModel(std::filesystem::directory_entry fileEntry) {
    std::ifstream file(fileEntry.path());
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }
    std::string line;
    unsigned int fIndex = 0;
    while (std::getline(file, line) && fIndex < 6) {
        loadTexture(line);
        blockTextures[blockCount][fIndex] = textureMap[line];
        ++fIndex;
    }
    file.close();
    blockMap[fileEntry.path().stem().string()] = blockCount++;
}

void loadTexture(std::string fileName) {
    if (textureMap.contains(fileName)) return;
    int width, height, nrChannels;
    unsigned char* data = stbi_load(("rsrc/textures/block/" + fileName + ".png").c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Error loading image." << std::endl;
        return;
    }
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, textureCount, 16, 16, 1, nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);
    textureMap[fileName] = textureCount++;
}