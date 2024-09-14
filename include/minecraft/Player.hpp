#pragma once
#include "Camera.hpp"
#include "string.h"

class Player {
	Camera cam;
public:
	int selectedItemSlot;
	string hotbarItems[];

	Player();

	Camera getCamera();
};

