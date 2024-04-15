#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_s.hpp"
#include "minecraft/camera.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "minecraft/level.hpp"
#include <cmath>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <fstream>
#include "OpenGLSetup/globals.hpp"
#include "OpenGLSetup/block_vao.hpp"
#include "OpenGLSetup/skybox_vao.hpp"


// Global variables
int frameCount = 0;
double previousTime = 0;

Camera cam = Camera();
Level level = Level(time(nullptr));

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastX = 500, lastY = 500;
float yaw = -90, pitch = 0;
bool firstMouse = true;

unsigned int instanceVBO;
glm::ivec4 instanceArray[100000];

unordered_map<std::string, unsigned short> blockMap{};
short blockTextures[6][65535];
unsigned short blockCount = 0;

unordered_map<std::string, unsigned short> textureMap{};
unsigned short textureCount = 0;

unsigned int cubemapTexture;

void calculateAndPrintFPS() {
    double currentTime = glfwGetTime();
    double elapsedTime = currentTime - previousTime;
    if (elapsedTime > 1) { // Update FPS every second
        float fps = frameCount / (elapsedTime);
        std::cout << "FPS: " << fps << std::endl;
        previousTime = currentTime;
        frameCount = 0;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    cam.aspect = (float) width / height;
}

void processInput(GLFWwindow* window) {
    float cameraSpeed = 8 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.moveForward(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.moveForward(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.moveStrafe(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.moveStrafe(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cam.moveVertical(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        cam.moveVertical(-cameraSpeed);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        for (float i = 0; i < 5; i += .1) {
            glm::vec3 v = glm::vec3{ cam.pos + cam.forwardVec * i };
            glm::ivec3 clickPos{ (int) floor(v[0]), (int) floor(v[1]), (int) floor(v[2])};
            if (level.getBlockAt(clickPos)) {
                level.deleteBlock(clickPos);
                break;
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        for (float i = 0; i < 5; i += .1) {
            glm::vec3 v = glm::vec3{ cam.pos + cam.forwardVec * i };
            glm::ivec3 clickPos{ (int)floor(v[0]), (int)floor(v[1]), (int)floor(v[2]) };
            if (level.getBlockAt(clickPos)) {
                v = glm::vec3{ cam.pos + cam.forwardVec * (i - .1f) };
                level.addBlock(Block{ (int)floor(v[0]), (int)floor(v[1]), (int)floor(v[2]), "diamond_block" });
                break;
            }
        }
    }
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float xPos{ (float) xpos }, yPos{ (float) ypos };
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.99f)
        pitch = 89.99f;
    if (pitch < -89.99f)
        pitch = -89.99f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cam.setLookingAt(glm::normalize(direction));
}


int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Minecraft", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1000, 1000);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    Shader ourShader("shader/shader.vs", "shader/shader.fs");
    Shader skyBoxShader("shader/cubeMap.vs", "shader/cubeMap.fs");
    ourShader.use();

    

    unsigned int VAO = createBlockVAO();
    glBindVertexArray(VAO);
    unsigned int skyboxVAO = createSkyboxVAO();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        cam.updateFrustum();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int x = -4; x < 5; ++x) {
            for (int z = -5; z < 5; ++z) {
                glm::ivec2 chunkPos{ (int)floor(cam.pos[0] / 16) + x, (int)floor(cam.pos[2] / 16) + z};
                if (!level.chunks.contains(chunkPos)) level.generateChunk(chunkPos);
            }
        }
        

        ourShader.use();
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glm::mat4 view = glm::lookAt(cam.pos, cam.pos + cam.forwardVec, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float) width / height, 0.1f, 1000.0f);
        glm::mat4 cameraTransform = projection * view;
    
        int cameraTransformLoc = glGetUniformLocation(ourShader.ID, "cameraTransform");
        glUniformMatrix4fv(cameraTransformLoc, 1, GL_FALSE, glm::value_ptr(cameraTransform));

        glBindVertexArray(VAO);
        unsigned int blockCount = 0;
        unsigned int totalBlocks = 0;
        unsigned int chunkCount = 0;
        unsigned int totalChunks = 0;
        for (auto const& pair1 : level.chunks) {
            const Chunk& chunk = pair1.second;
            ++totalChunks;
            if (!cam.isVisible(chunk)) continue;
            if (abs(chunk.x - cam.pos[0] / 16) > 4 || abs(chunk.z - cam.pos[2] / 16) > 4) continue;
            ++chunkCount;
            for (auto const& pair2: chunk.blocks) {
                ++totalBlocks;
                const Block& block = pair2.second;
                if (block.cullCoveredFace.all()) continue;
                if (!cam.isVisible(block)) continue;

                for (int i = 0; i < 6; ++i) {
                    int y = (block.y() << 16) | blockTextures[blockMap[block.type]][i];
                    
                    if (block.cullCoveredFace[i]) continue;
                    int lighting = i / 2;
                    int data = (i << 8) | (lighting);
                    instanceArray[blockCount] = glm::ivec4{ block.x(), y, block.z(), data };
                    ++blockCount;
                }

            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, 100000 * sizeof(int) * 4, 0, GL_STREAM_DRAW);
        void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr, instanceArray, blockCount * 4 * sizeof(int));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glUniform1i(glGetUniformLocation(ourShader.ID, "textureArray"), 0);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, blockCount);

        

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyBoxShader.use();
        GLuint viewLoc = glGetUniformLocation(skyBoxShader.ID, "view");
        view = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        GLuint projectionLoc = glGetUniformLocation(skyBoxShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);


        ++frameCount;
        calculateAndPrintFPS();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}