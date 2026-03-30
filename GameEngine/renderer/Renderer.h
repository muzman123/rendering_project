#pragma once
#include "Shader.h"
#include "Camera.h"
#include "Skybox.h"
#include "Terrain.h"
#include "TerrainManager.h"
#include "../scene/Scene.h"

class Renderer {
public:
    bool init();
    void render(const Scene& scene, float aspect);
    void shutdown();

    Skybox& getSkybox() { return m_skybox; }
    TerrainGenerator& getTerrainGenerator() { return m_terrainGen; }
    WorldStreamer& getWorldStreamer() { return m_worldStreamer; }

private:
    Shader m_shader;
    Skybox m_skybox;
    TerrainGenerator m_terrainGen;
    WorldStreamer m_worldStreamer;
};