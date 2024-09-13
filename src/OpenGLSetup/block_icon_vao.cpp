#include "OpenGLSetup/block_icon_vao.hpp"
#include "OpenGLSetup/globals.hpp"

GLuint createBlockIconVAO() {
    GLuint vertices[] = { 0,1,2,3, 4,5,6,7, 8,9,10,11 };

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLuint), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &blockIconVBO);
    glBindBuffer(GL_ARRAY_BUFFER, blockIconVBO);
    glBufferData(GL_ARRAY_BUFFER, 256 * sizeof(int) * 4, nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(int), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    return VAO;
}