#pragma once
#include <string>
#include "Transform.h"
#include "../renderer/Mesh.h"

// An Entity is anything that exists in the world.
// It has a transform (where it is) and a mesh (what it looks like).
// This is like a simplified Unity GameObject.

class Entity {
public:
    std::string name;
    Transform transform;
    Mesh* mesh = nullptr;      // What to draw (can be shared between entities)
    bool rotates = false;      // Simple behavior flag for now
    float rotateSpeed = 0.0f;

    void update(float dt, float time) {
        if (rotates) {
            transform.rotation.y = time * rotateSpeed;
        }
    }
};