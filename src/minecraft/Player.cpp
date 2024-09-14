#include "minecraft/Player.hpp"


Player::Player() : cam{}, selectedItemSlot{ 0 }, hotbarItems{"stone"} {}

Camera Player::getCamera() {
	return cam;
}