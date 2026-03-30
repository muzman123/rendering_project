#pragma once
#include <glad/glad.h>
#include "Shader.h"
#include "Camera.h"

// The skybox is a cube rendered around the camera with depth
// testing disabled so it's always behind everything.
// Instead of loading a cubemap texture, we generate the sky
// colors procedurally in the shader — blue up top, warm horizon.

class Skybox {
public:
    bool init();
    void draw(const Camera& camera, float aspect);
    void destroy();

private:
    unsigned int m_VAO = 0, m_VBO = 0;
    Shader m_shader;
};