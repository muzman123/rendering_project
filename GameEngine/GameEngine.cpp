#define SDL_MAIN_HANDLED
#include "core/Engine.h"
#include "scene/Scene.h"
#include <random> 

// Bootstraps initial scene components for testing out rendering engine limits and scale.
void BuildDemoScene(Scene& scene, Engine& engine) {
    scene.lights.push_back({
        glm::vec3(100.0f, 150.0f, 60.0f),
        glm::vec3(1.0f, 0.95f, 0.8f)
        });
    scene.lights.push_back({
        glm::vec3(-80.0f, 80.0f, -40.0f),
        glm::vec3(0.3f, 0.4f, 0.6f)
        });
    scene.lights.push_back({
        glm::vec3(-40.0f, 40.0f, -20.0f),
        glm::vec3(0.3f, 0.4f, 0.6f)
        });

    Entity& tower = scene.addEntity("Tower");
    tower.mesh = scene.addMesh(Mesh::createCube({ 0.8f, 0.15f, 0.15f }));
    tower.transform.position = { 0, 4, 0 };
    tower.transform.scale = { 1, 8, 1 };

    for (int i = 0; i < 6; i++) {
        float angle = i * 3.14159f * 2.0f / 6.0f;
        float radius = 12.0f;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        Entity& pillar = scene.addEntity("Pillar " + std::to_string(i));
        pillar.mesh = scene.addMesh(Mesh::createCube({ 0.55f, 0.5f, 0.45f }));

        float groundY = engine.getRenderer().getTerrainGenerator().getHeight(x, z);
        pillar.transform.position = { x, groundY + 2.5f, z };
        pillar.transform.scale = { 0.8f, 5.0f, 0.8f };
        pillar.transform.rotation.y = angle * 57.3f; 
    }

    struct ObjDef { glm::vec3 pos; glm::vec3 color; float scale; float rotSpeed; };
    std::vector<ObjDef> objects = {
        {{ 15, 0, -10},  {0.2f, 0.3f, 0.85f}, 2.0f,  20.0f}, 
        {{-20, 0,  15},  {0.2f, 0.75f, 0.25f}, 1.5f,  35.0f},
        {{ 30, 0,  25},  {0.85f, 0.65f, 0.2f}, 1.8f, -15.0f},  
        {{-35, 0, -25},  {0.6f, 0.15f, 0.7f},  2.5f,  10.0f},  
        {{ 45, 0, -35},  {0.9f, 0.9f, 0.9f},   1.2f,  45.0f}, 
        {{-15, 0,  40},  {0.85f, 0.3f, 0.2f},  1.6f, -25.0f},  
        {{ 25, 0,  50},  {0.2f, 0.7f, 0.7f},   2.0f,  30.0f}, 
        {{-50, 0, -10},  {0.7f, 0.7f, 0.2f},   1.4f,  55.0f}, 
    };

    auto& terrainGen = engine.getRenderer().getTerrainGenerator();
    for (auto& obj : objects) {
        Entity& e = scene.addEntity("Cube");
        e.mesh = scene.addMesh(Mesh::createCube(obj.color));

        float groundY = terrainGen.getHeight(obj.pos.x, obj.pos.z);
        e.transform.position = { obj.pos.x, groundY + obj.scale * 0.5f, obj.pos.z };
        e.transform.scale = glm::vec3(obj.scale);
        e.rotates = true;
        e.rotateSpeed = obj.rotSpeed;
    }

    float startY = terrainGen.getHeight(0, 15) + 2.0f;
    scene.camera.position = glm::vec3(0, startY, 15);
    scene.camera.moveSpeed = 25.0f;
}

int main(int argc, char* argv[]) {
    Engine engine;
    if (!engine.init("Open World Explorer", 1280, 720)) return 1;

    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<unsigned int> seedDist(0, 9999999);
    unsigned int randomSeed = seedDist(gen);

    engine.getRenderer().getTerrainGenerator().init(randomSeed);

    Scene scene;
    BuildDemoScene(scene, engine);

    engine.run(scene);
    engine.shutdown(scene);

    return 0;
}