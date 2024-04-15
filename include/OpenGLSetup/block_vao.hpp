#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "stb_image.h"
#include "globals.hpp"

GLuint createBlockVAO();

void loadModels(std::string folderPath);
void loadModel(std::filesystem::directory_entry fileEntry);
void loadTexture(std::string fileName);