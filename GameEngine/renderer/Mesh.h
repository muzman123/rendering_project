#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color; // Changed to vec4!
};

class Mesh {
public:
    void create(const std::vector<Vertex>& vertices);
    void draw() const;
    void destroy();

    static Mesh createCube(const glm::vec3& color);
    static Mesh createPlane(float size, int tiles,
        const glm::vec3& color1, const glm::vec3& color2);

private:
    unsigned int m_VAO = 0, m_VBO = 0;
    int m_vertexCount = 0;
};