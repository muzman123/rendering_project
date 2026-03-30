#include "Camera.h"
#include "../core/Input.h"
#include <cmath>

glm::vec3 Camera::getForward() const {
    float yr = glm::radians(yaw), pr = glm::radians(pitch);
    return glm::normalize(glm::vec3(
        cos(pr) * cos(yr), sin(pr), cos(pr) * sin(yr)
    ));
}

glm::vec3 Camera::getRight() const {
    return glm::normalize(glm::cross(getForward(), glm::vec3(0, 1, 0)));
}

void Camera::update(const Input& input, float dt) {
    // Mouse look
    yaw += input.getMouseDX() * lookSpeed;
    pitch -= input.getMouseDY() * lookSpeed;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Movement
    glm::vec3 fwd = getForward();
    glm::vec3 right = getRight();
    float spd = moveSpeed * dt;

    if (input.isKeyDown(SDL_SCANCODE_W)) position += fwd * spd;
    if (input.isKeyDown(SDL_SCANCODE_S)) position -= fwd * spd;
    if (input.isKeyDown(SDL_SCANCODE_A)) position -= right * spd;
    if (input.isKeyDown(SDL_SCANCODE_D)) position += right * spd;
    if (input.isKeyDown(SDL_SCANCODE_SPACE)) position.y += spd;
    if (input.isKeyDown(SDL_SCANCODE_LSHIFT)) position.y -= spd;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + getForward(), glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(fov), aspect, 0.1f, 500.0f);
}