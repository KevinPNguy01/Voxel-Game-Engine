#include "OpenGLSetup/gui_vao.hpp"

GLuint createGuiVAO() {
    // Generate and bind the VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &guiVBO);
    glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(int) * 4, nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Generate texture
    unsigned int atlas;
    glGenTextures(1, &atlas);
    glBindTexture(GL_TEXTURE_2D, atlas);

    // Load image data
    int width, height, nrChannels;
    unsigned char* imageData = stbi_load("rsrc/textures/gui/widgets.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    free(imageData);

    // Set texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return VAO;
}