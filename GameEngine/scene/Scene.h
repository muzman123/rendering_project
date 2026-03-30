#pragma once
#include <vector>
#include <memory>
#include "Entity.h"
#include "../renderer/Mesh.h"
#include "../renderer/Shader.h"
#include "../renderer/Camera.h"

struct Light {
    glm::vec3 position;
    glm::vec3 color;
};

class Scene {
public:
    Camera camera;
    std::vector<Entity> entities;
    std::vector<Light> lights;
    std::vector<Mesh> meshes;  // Owns mesh data (entities point into this)

    // Add a mesh and return a pointer to it (for entities to reference)
    Mesh* addMesh(Mesh mesh) {
        meshes.push_back(std::move(mesh));
        return &meshes.back();
    }

    Entity& addEntity(const std::string& name) {
        entities.push_back({});
        entities.back().name = name;
        return entities.back();
    }

    void update(float dt, float time) {
        for (auto& e : entities) {
            e.update(dt, time);
        }
    }

    void destroy() {
        for (auto& m : meshes) m.destroy();
    }
};