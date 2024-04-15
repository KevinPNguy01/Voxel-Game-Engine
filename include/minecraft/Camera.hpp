#ifndef CAMERA
#define CAMERA

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Chunk.hpp"

struct Plane {
	glm::vec3 point;
	glm::vec3 normal;

	int distance(glm::vec3 p);
};

struct Frustum {
	Plane near, far, top, bottom, left, right;
};

class Camera {
	Frustum frustum;

public:
	float aspect;
	float fov;

	glm::vec3 pos;
	glm::vec3 forwardVec;
	glm::vec3 upVec;
	glm::vec3 leftVec;

	Camera();

	void setLookingAt(glm::vec3 direction);
	void updateFrustum();

	bool isVisible(const Chunk& c);
	bool isVisible(const Block& b);

	void moveForward(float amount);
	void moveStrafe(float amount);
	void moveVertical(float amount);

	float x() const;
	float y() const;
	float z() const;
};

#endif