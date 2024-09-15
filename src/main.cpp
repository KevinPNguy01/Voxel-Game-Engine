#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_s.hpp"
#include "minecraft/camera.hpp"
#include "minecraft/player.hpp"
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
#include <tuple>
#include <array>
#include "OpenGLSetup/globals.hpp"
#include "OpenGLSetup/block_vao.hpp"
#include "OpenGLSetup/skybox_vao.hpp"
#include "OpenGLSetup/gui_vao.hpp"
#include "OpenGLSetup/block_icon_vao.hpp"


// Global variables
int frameCount = 0;
double previousTime = 0;

Player player = Player();
Camera cam = player.getCamera();
Level level = Level(time(nullptr));

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastX = 500, lastY = 500;
float yaw = -90, pitch = 0;
bool firstMouse = true;

GLuint textureArray;

unsigned int guiVBO;
glm::vec4 guiArray[256];

unsigned int instanceVBO;
glm::ivec4 instanceArray[100000];

unsigned int blockIconVBO;
glm::ivec4 blockIconArray[256];

unordered_map<std::string, unsigned short> blockMap{};
unsigned short blockTextures[65535][6];
unsigned short blockCount = 0;

unordered_map<std::string, unsigned short> textureMap{};
unsigned short textureCount = 0;

unsigned int cubemapTexture;

static std::tuple<glm::vec4, glm::vec4, glm::vec4, glm::vec4> createQuad(float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2) {
    return {
        glm::vec4(x1, y1, u1, v1),
        glm::vec4(x2, y1, u2, v1),
        glm::vec4(x2, y2, u2, v2),
        glm::vec4(x1, y2, u1, v2)
    };
}

static std::tuple<glm::vec4, glm::vec4, glm::vec4, glm::vec4> createQuadSized(float x, float y, float w, float h, float u, float v, float tw, float th) {
    return createQuad(x, y, x + w, y + h, u, v + th, u + tw, v);
}

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
    // Movement keys.
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

    // Hotbar keys.
    for (int key = GLFW_KEY_1; key <= GLFW_KEY_9; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS) {
            player.selectedItemSlot = key - GLFW_KEY_1;
        }
    }
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
                level.addBlock(Block{ (int)floor(v[0]), (int)floor(v[1]), (int)floor(v[2]), player.getHotbarItems()[player.selectedItemSlot]});
                break;
            }
        }
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        // Hold control while scrolling to cycle through the available block times in the current selected slot.
        int blockId = (blockMap[player.getHotbarItems()[player.selectedItemSlot]] - (int)yOffset) % blockCount;
        if (blockId < 0) {
            blockId += blockCount;
        }
        string block = "diamond_block";
        for (auto it = blockMap.begin(); it != blockMap.end(); ++it) {
            if (it->second == blockId) {
                player.getHotbarItems()[player.selectedItemSlot] = it->first;
            }
        }
    } else {
        // Otherwise, scroll through the hotbar slots.
        player.selectedItemSlot = (player.selectedItemSlot - (int) yOffset) % 9;
        if (player.selectedItemSlot < 0) {
            player.selectedItemSlot += 9;
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
    glfwSetScrollCallback(window, scrollCallback);

    Shader ourShader("shader/shader.vs", "shader/shader.fs");
    Shader skyBoxShader("shader/cubeMap.vs", "shader/cubeMap.fs");
    Shader guiShader("shader/gui.vs", "shader/gui.fs");
    Shader blockIconShader("shader/blockIcon.vs", "shader/blockIcon.fs");
    ourShader.use();

    

    unsigned int VAO = createBlockVAO();
    unsigned int skyboxVAO = createSkyboxVAO();
    unsigned int guiVAO = createGuiVAO();
    unsigned int blockIconVAO = createBlockIconVAO();

    
    glBindVertexArray(VAO);


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        cam.updateFrustum();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Generate new chunks nearby.
        for (int x = -4; x < 5; ++x) {
            for (int z = -4; z < 5; ++z) {
                glm::ivec2 chunkPos{ (int)floor(cam.pos[0] / 16) + x, (int)floor(cam.pos[2] / 16) + z};
                if (!level.chunks.contains(chunkPos)) level.generateChunk(chunkPos);
            }
        }
        

        ourShader.use();
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspectRatio = (float) width / height;

        // Render visible blocks.
        glm::mat4 view = glm::lookAt(cam.pos, cam.pos + cam.forwardVec, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float) width / height, 0.1f, 1000.0f);
        glm::mat4 cameraTransform = projection * view;
    
        int cameraTransformLoc = glGetUniformLocation(ourShader.ID, "cameraTransform");
        glUniformMatrix4fv(cameraTransformLoc, 1, GL_FALSE, glm::value_ptr(cameraTransform));

        glBindVertexArray(VAO);
        unsigned int blockCount = 0;    // Total blocks rendered.
        unsigned int totalBlocks = 0;   // Total blocks checked.
        unsigned int chunkCount = 0;    // Total chunks rendered.
        unsigned int totalChunks = 0;   // Total chunks checked.
        // Iterate through all chunks.
        for (auto const& pair1 : level.chunks) {
            const Chunk& chunk = pair1.second;
            ++totalChunks;

            // Skip chunks that are out of viewing frustum, or too far.
            if (!cam.isVisible(chunk)) continue;
            if (abs(chunk.x - cam.pos[0] / 16) > 4 || abs(chunk.z - cam.pos[2] / 16) > 4) continue;
            ++chunkCount;

            // Iterate through all blocks.
            for (auto const& pair2: chunk.blocks) {
                const Block& block = pair2.second;
                ++totalBlocks;

                // Skip blocks that are out of viewing frustum, or covered on all sides.
                if (block.cullCoveredFace.all()) continue;
                if (!cam.isVisible(block)) continue;

                // Iterate through all faces.
                for (int i = 0; i < 6; ++i) {
                    // Skip covered faces.
                    if (block.cullCoveredFace[i]) continue;

                    // Pack y-level and texture id into a single integer.
                    int y = (block.y() << 16) | blockTextures[blockMap[block.type]][i];

                    // Pack face index and lighting index into a single integer.
                    int lighting = i;
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
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, blockCount);

        
        // Render skybox.
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


        // GUI

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        guiShader.use();
        glBindVertexArray(guiVAO);

        // Hotbar.

        float slotSize = aspectRatio / (182.f / 20);
        float pixelX = 1.f / 182;
        float pixelY = pixelX * aspectRatio;

        // Bar should take up half the width of the screen, rendered at the bottom.

        auto hotbarQuad = createQuadSized(-0.5, -1 + pixelY, 182 * pixelX, 22 * pixelY, 0, 0, 182.f / 256, 22.f / 256);
        guiArray[0] = std::get<0>(hotbarQuad);
        guiArray[1] = std::get<1>(hotbarQuad);
        guiArray[2] = std::get<2>(hotbarQuad);
        guiArray[3] = std::get<3>(hotbarQuad);


        // Selected slot.
        auto selectedSlotQuad = createQuadSized(-24.f * pixelX / 2 + (-4 + player.selectedItemSlot) * 20 * pixelX, -1, 24 * pixelX, 24 * pixelY, 0, 22.f / 256, 24.f / 256, 24.f / 256);
        guiArray[4] = std::get<0>(selectedSlotQuad);
        guiArray[5] = std::get<1>(selectedSlotQuad);
        guiArray[6] = std::get<2>(selectedSlotQuad);
        guiArray[7] = std::get<3>(selectedSlotQuad);

        // Crosshair.
        auto crosshairQuad = createQuadSized(-9 * pixelX / 2, -9 * pixelY / 2, 9 * pixelX, 9 * pixelY, 243.f / 256, 3.f / 256, 9.f / 256, 9.f / 256);
        guiArray[8] = std::get<0>(crosshairQuad);
        guiArray[9] = std::get<1>(crosshairQuad);
        guiArray[10] = std::get<2>(crosshairQuad);
        guiArray[11] = std::get<3>(crosshairQuad);

        glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
        glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(int), 0, GL_STREAM_DRAW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr, guiArray, 48 * sizeof(int));
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Draw hotbar
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4); // Draw selected slot
        glDrawArrays(GL_TRIANGLE_FAN, 8, 4); // Draw crosshair

        glDisable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Hotbar Icons
        
        blockIconShader.use();
        blockIconShader.setFloat("slotSize", slotSize/2);
        blockIconShader.setFloat("aspectRatio", aspectRatio);
        glBindVertexArray(blockIconVAO);

        // Render hotbar items.
        std::array<string, 9> hotbarItems = player.getHotbarItems();
        size_t blockIconCount;
        for (blockIconCount = 0; blockIconCount < hotbarItems.size(); ++blockIconCount) {
            string block = hotbarItems[blockIconCount];
            unsigned short* textures = blockTextures[(int)blockMap[block]];
            blockIconArray[blockIconCount] = glm::uvec4((textures[1] << 8) | textures[3], textures[5], blockIconCount, 4);
        }

        glBindBuffer(GL_ARRAY_BUFFER, blockIconVBO);
        glBufferData(GL_ARRAY_BUFFER, blockIconCount * sizeof(int) * 4, 0, GL_STREAM_DRAW);
        ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr, blockIconArray, blockIconCount * 4 * sizeof(int));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        // Render each of the three faces of each icon.
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, blockIconCount);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 4, 4, blockIconCount);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 8, 4, blockIconCount);
        

        glEnable(GL_CULL_FACE);

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