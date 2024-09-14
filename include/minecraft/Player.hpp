#pragma once
#include "Camera.hpp"
#include "string.h"
#include <array>

class Player {
	Camera cam;
	std::array<std::string, 9> hotbarItems;
public:
	int selectedItemSlot;

	Player();

	Camera getCamera();

	std::array<std::string, 9> getHotbarItems();
};

