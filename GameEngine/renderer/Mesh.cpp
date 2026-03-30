#include "Mesh.h"

void Mesh::create(const std::vector<Vertex>& vertices) {
    m_vertexCount = (int)vertices.size();

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Color: Change size from 3 to 4
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}

void Mesh::destroy() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

Mesh Mesh::createCube(const glm::vec3& c) {
    glm::vec4 c4(c, 1.0f); // Convert vec3 to opaque vec4
    std::vector<Vertex> verts = {
        // Front
        {{-0.5f,-0.5f, 0.5f},{0,0,1},c4}, {{ 0.5f,-0.5f, 0.5f},{0,0,1},c4},
        {{ 0.5f, 0.5f, 0.5f},{0,0,1},c4}, {{ 0.5f, 0.5f, 0.5f},{0,0,1},c4},
        {{-0.5f, 0.5f, 0.5f},{0,0,1},c4}, {{-0.5f,-0.5f, 0.5f},{0,0,1},c4},
        // Back
        {{-0.5f,-0.5f,-0.5f},{0,0,-1},c4}, {{-0.5f, 0.5f,-0.5f},{0,0,-1},c4},
        {{ 0.5f, 0.5f,-0.5f},{0,0,-1},c4}, {{ 0.5f, 0.5f,-0.5f},{0,0,-1},c4},
        {{ 0.5f,-0.5f,-0.5f},{0,0,-1},c4}, {{-0.5f,-0.5f,-0.5f},{0,0,-1},c4},
        // Top
        {{-0.5f, 0.5f,-0.5f},{0,1,0},c4}, {{-0.5f, 0.5f, 0.5f},{0,1,0},c4},
        {{ 0.5f, 0.5f, 0.5f},{0,1,0},c4}, {{ 0.5f, 0.5f, 0.5f},{0,1,0},c4},
        {{ 0.5f, 0.5f,-0.5f},{0,1,0},c4}, {{-0.5f, 0.5f,-0.5f},{0,1,0},c4},
        // Bottom
        {{-0.5f,-0.5f,-0.5f},{0,-1,0},c4}, {{ 0.5f,-0.5f,-0.5f},{0,-1,0},c4},
        {{ 0.5f,-0.5f, 0.5f},{0,-1,0},c4}, {{ 0.5f,-0.5f, 0.5f},{0,-1,0},c4},
        {{-0.5f,-0.5f, 0.5f},{0,-1,0},c4}, {{-0.5f,-0.5f,-0.5f},{0,-1,0},c4},
        // Right
        {{ 0.5f,-0.5f,-0.5f},{1,0,0},c4}, {{ 0.5f, 0.5f,-0.5f},{1,0,0},c4},
        {{ 0.5f, 0.5f, 0.5f},{1,0,0},c4}, {{ 0.5f, 0.5f, 0.5f},{1,0,0},c4},
        {{ 0.5f,-0.5f, 0.5f},{1,0,0},c4}, {{ 0.5f,-0.5f,-0.5f},{1,0,0},c4},
        // Left
        {{-0.5f,-0.5f,-0.5f},{-1,0,0},c4}, {{-0.5f,-0.5f, 0.5f},{-1,0,0},c4},
        {{-0.5f, 0.5f, 0.5f},{-1,0,0},c4}, {{-0.5f, 0.5f, 0.5f},{-1,0,0},c4},
        {{-0.5f, 0.5f,-0.5f},{-1,0,0},c4}, {{-0.5f,-0.5f,-0.5f},{-1,0,0},c4},
    };

    Mesh mesh;
    mesh.create(verts);
    return mesh;
}

Mesh Mesh::createPlane(float size, int tiles,
    const glm::vec3& color1, const glm::vec3& color2) {
    std::vector<Vertex> verts;
    float tileSize = size / tiles;
    float half = size / 2.0f;
    glm::vec3 n(0, 1, 0);

    for (int z = 0; z < tiles; z++) {
        for (int x = 0; x < tiles; x++) {
            float x0 = -half + x * tileSize, z0 = -half + z * tileSize;
            float x1 = x0 + tileSize, z1 = z0 + tileSize;
            
            // Get original vec3 color
            glm::vec3 c = ((x + z) % 2 == 0) ? color1 : color2;
            
            // Convert to vec4 with 1.0f alpha (fully opaque)
            glm::vec4 c4(c, 1.0f);

            verts.push_back({ {x0,0,z0}, n, c4 });
            verts.push_back({ {x1,0,z0}, n, c4 });
            verts.push_back({ {x1,0,z1}, n, c4 });
            verts.push_back({ {x1,0,z1}, n, c4 });
            verts.push_back({ {x0,0,z1}, n, c4 });
            verts.push_back({ {x0,0,z0}, n, c4 });
        }
    }

    Mesh mesh;
    mesh.create(verts);
    return mesh;
}