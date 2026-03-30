#pragma once
#include "Mesh.h"
#include "PerlinNoise.h"
#include <glm/glm.hpp>
#include <vector>

constexpr float CHUNK_SIZE = 64.0f;
constexpr int CHUNK_RES = 65;

class TerrainGenerator {
public:
    void init(unsigned int seed = 42);

    void generateChunk(Mesh& outTerrainMesh, Mesh& outWaterMesh, bool& outHasWater, int chunkX, int chunkZ) const;

    float getHeight(float worldX, float worldZ) const;

private:
    PerlinNoise m_noise;
    PerlinNoise m_biomeNoise;
    
    float m_offsetX = 0;
    float m_offsetZ = 0;

    float computeHeight(float worldX, float worldZ, float& outWaterLevel) const;
    glm::vec3 computeColor(float height, float waterLevel) const;
};