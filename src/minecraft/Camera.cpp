#include "minecraft/camera.hpp"

// Returns the signed distance from this plane to point p.
int Plane::distance(glm::vec3 p) {
	return glm::dot((p - point), normal);
}

Camera::Camera() :
	frustum{ },
	pos{ 0, 0, 0 },
	aspect{ 1 },
	fov{ 90 }
{
	setLookingAt(glm::vec3(0.0f, 0.0f, -1.0f));
	updateFrustum();
}

// Sets the camera to be looking in the direction of the given vector.
void Camera::setLookingAt(glm::vec3 direction) {
	forwardVec = glm::normalize(direction);
	leftVec = glm::normalize(glm::cross(forwardVec, glm::vec3{ 0,-1,0 }));
	upVec = glm::normalize(glm::cross(forwardVec, leftVec));
}

// Updates the camera frustum.
void Camera::updateFrustum() {
	float nearDist{ 0.1f }, farDist{ 32 };
	float fov = glm::radians(this->fov);
	float Hnear{ tan(fov / 2) * nearDist }, Wnear{ Hnear * aspect };
	float Hfar{ tan(fov / 2) * farDist }, Wfar{ Hfar * aspect };
	glm::vec3 nearCenter{ forwardVec * nearDist }, farCenter{ forwardVec * farDist };

	frustum.near.point = pos + nearCenter;
	frustum.near.normal = forwardVec;

	frustum.far.point = pos + farCenter;
	frustum.far.normal = -forwardVec;

	frustum.left.point = pos;
	frustum.left.normal = glm::normalize(glm::cross(nearCenter + leftVec * Wnear, upVec));

	frustum.right.point = pos;
	frustum.right.normal = glm::normalize(glm::cross(upVec, nearCenter - leftVec * Wnear));

	frustum.top.point = pos;
	frustum.top.normal = glm::normalize(glm::cross(leftVec, nearCenter + upVec * Hnear));

	frustum.bottom.point = pos;
	frustum.bottom.normal = glm::normalize(glm::cross(nearCenter - upVec * Hnear, leftVec));
}

// Returns true if a block is at least partially visible in the camera frustum.
bool Camera::isVisible(const Block& b) {
	glm::vec3 p{ b.x() + 0.5f, b.y() + 0.5f, b.z() + 0.5f };
	bool leftRight = min(frustum.left.distance(p), frustum.right.distance(p)) > -0.866;
	bool topBottom = min(frustum.top.distance(p), frustum.bottom.distance(p)) > -0.866;
	bool near = frustum.near.distance(p) > -0.866;
	return leftRight && topBottom && near;
}

// Returns true if a chunk is at least partially visible in the camera frustum.
bool Camera::isVisible(const Chunk& c) {
	glm::vec3 p{ c.x * 16 + 8, y() + forwardVec.y * 256, c.z * 16 + 8};
	return min(frustum.left.distance(p), frustum.right.distance(p)) > -11.314;
}

void Camera::moveForward(float amount) {
	pos += glm::normalize(forwardVec * glm::vec3(1, 0, 1)) * amount;
}
void Camera::moveStrafe(float amount) {
	pos += leftVec * amount;
}
void Camera::moveVertical(float amount) {
	pos += glm::vec3{0, 1, 0} *amount;
}

float Camera::x() const { return pos[0]; }
float Camera::y() const { return pos[1]; }
float Camera::z() const { return pos[2]; }