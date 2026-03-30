#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Input;

class Camera {
public:
    glm::vec3 position = glm::vec3(0, 2, 8);
    float yaw = -90.0f;
    float pitch = -10.0f;
    float moveSpeed = 5.0f;
    float lookSpeed = 0.15f;
    float fov = 60.0f;

    void update(const Input& input, float dt);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;

    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
};