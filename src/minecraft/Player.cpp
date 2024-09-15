#include "minecraft/Player.hpp"
#include <array>

Player::Player() : 
	cam{}, 
	selectedItemSlot{ 4 }, 
	hotbarItems{"grass_block", "dirt", "stone", "cobblestone", "gravel", "sand", "sandstone", "bricks", "oak_planks"}
{}

Camera Player::getCamera() {
	return cam;
}

std::array<std::string, 9>& Player::getHotbarItems() {
	return hotbarItems;
}