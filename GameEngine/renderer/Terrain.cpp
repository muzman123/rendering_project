#include "Terrain.h"
#include <cmath>
#include <algorithm>
#include <random>
    
void TerrainGenerator::init(unsigned int seed) {
    m_noise = PerlinNoise(seed);
    m_biomeNoise = PerlinNoise(seed + 12345);

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-100000.0f, 100000.0f);
    
    m_offsetX = dist(rng);
    m_offsetZ = dist(rng);
}

float TerrainGenerator::computeHeight(float worldX, float worldZ, float& outWaterLevel) const {
    worldX += m_offsetX;
    worldZ += m_offsetZ;

    float rawBiomeValue = m_biomeNoise.fbm(worldX * 0.0015f, worldZ * 0.0015f, 3, 2.0f, 0.5f);
    
    // We use smoothstep to enforce stark biome transitions. 
    // Standard interpolation resulted in too much muddy, undefined terrain.
    float biomeValue = glm::smoothstep(0.35f, 0.65f, rawBiomeValue);

    float localMaxHeight = glm::mix(8.0f, 85.0f, biomeValue);
    outWaterLevel = glm::mix(4.0f, 15.0f, biomeValue); 

    float continent = m_noise.fbm(worldX * 0.01f, worldZ * 0.01f, 4, 2.0f, 0.5f);
    float hills = m_noise.fbm(worldX * 0.03f, worldZ * 0.03f, 4, 2.0f, 0.5f);
    float detail = m_noise.fbm(worldX * 0.15f, worldZ * 0.15f, 3, 2.0f, 0.4f);
    
    float ridgeNoise = m_noise.fbm(worldX * 0.02f, worldZ * 0.02f, 4, 2.0f, 0.5f);
    float ridge = 1.0f - std::abs(ridgeNoise * 2.0f - 1.0f);
    ridge = std::pow(ridge, 2.0f);

    float plainsShape = continent * 0.7f + hills * 0.25f + detail * 0.05f;
    float mountainShape = continent * 0.3f + ridge * 0.5f + hills * 0.15f + detail * 0.05f;

    float height = glm::mix(plainsShape, mountainShape, biomeValue);
    height = std::pow(height, 1.2f);
    height = std::max((height - 0.2f) * 1.25f, 0.0f);

    return height * localMaxHeight;
}

glm::vec3 TerrainGenerator::computeColor(float height, float waterLevel) const {
    if (height < waterLevel - 0.2f) return glm::vec3(0.15f, 0.25f, 0.35f);
    if (height < waterLevel + 0.5f) return glm::vec3(0.76f, 0.7f, 0.5f);

    float t = height / 90.0f; // Normalized against absolute max possible height

    if (t < 0.35f) {
        float blend = t / 0.35f;
        return glm::mix(glm::vec3(0.76f, 0.7f, 0.5f), glm::vec3(0.22f, 0.52f, 0.15f), blend);
    }
    if (t < 0.5f) {
        return glm::mix(glm::vec3(0.22f, 0.52f, 0.15f), glm::vec3(0.15f, 0.38f, 0.1f), (t - 0.35f) / 0.15f);
    }
    if (t < 0.7f) {
        return glm::mix(glm::vec3(0.15f, 0.38f, 0.1f), glm::vec3(0.45f, 0.38f, 0.3f), (t - 0.5f) / 0.2f);
    }
    if (t < 0.85f) {
        return glm::mix(glm::vec3(0.45f, 0.38f, 0.3f), glm::vec3(0.55f, 0.52f, 0.48f), (t - 0.7f) / 0.15f);
    }
    
    return glm::mix(
        glm::vec3(0.55f, 0.52f, 0.48f), 
        glm::vec3(0.85f, 0.87f, 0.9f), 
        glm::clamp((t - 0.85f) / 0.15f, 0.0f, 1.0f)
    );
}

// This queries height arbitrarily, disregarding the fluid sim level.
float TerrainGenerator::getHeight(float worldX, float worldZ) const {
    float dummyWaterLevel;
    return computeHeight(worldX, worldZ, dummyWaterLevel);
}

void TerrainGenerator::generateChunk(Mesh& outTerrainMesh, Mesh& outWaterMesh, bool& outHasWater, int chunkX, int chunkZ) const {
    float step = CHUNK_SIZE / (float)(CHUNK_RES - 1);
    float startX = chunkX * CHUNK_SIZE;
    float startZ = chunkZ * CHUNK_SIZE;
    
    std::vector<glm::vec3> positions(CHUNK_RES * CHUNK_RES);
    std::vector<float> localWaterLevels(CHUNK_RES * CHUNK_RES);

    for (int z = 0; z < CHUNK_RES; z++) {
        for (int x = 0; x < CHUNK_RES; x++) {
            float wx = startX + x * step;
            float wz = startZ + z * step;
            
            float localWater;
            float wy = computeHeight(wx, wz, localWater);
            
            int idx = z * CHUNK_RES + x;
            positions[idx] = glm::vec3(wx, wy, wz);
            localWaterLevels[idx] = localWater;
        }
    }
    
    std::vector<Vertex> verts;
    for (int z = 0; z < CHUNK_RES - 1; z++) {
        for (int x = 0; x < CHUNK_RES - 1; x++) {
            int idxTL = z * CHUNK_RES + x;
            int idxTR = z * CHUNK_RES + x + 1;
            int idxBL = (z + 1) * CHUNK_RES + x;
            int idxBR = (z + 1) * CHUNK_RES + x + 1;

            glm::vec3 tl = positions[idxTL];
            glm::vec3 tr = positions[idxTR];
            glm::vec3 bl = positions[idxBL];
            glm::vec3 br = positions[idxBR];

            glm::vec3 n1 = glm::normalize(glm::cross(tr - tl, bl - tl));
            glm::vec3 n2 = glm::normalize(glm::cross(bl - tr, br - tr));

            glm::vec3 ctL = computeColor(tl.y, localWaterLevels[idxTL]);
            glm::vec3 ctR = computeColor(tr.y, localWaterLevels[idxTR]);
            glm::vec3 cbL = computeColor(bl.y, localWaterLevels[idxBL]);
            glm::vec3 cbR = computeColor(br.y, localWaterLevels[idxBR]);

            verts.push_back({ tl, n1, glm::vec4(ctL, 1.0f) });
            verts.push_back({ tr, n1, glm::vec4(ctR, 1.0f) });
            verts.push_back({ bl, n1, glm::vec4(cbL, 1.0f) });

            verts.push_back({ tr, n2, glm::vec4(ctR, 1.0f) });
            verts.push_back({ br, n2, glm::vec4(cbR, 1.0f) });
            verts.push_back({ bl, n2, glm::vec4(cbL, 1.0f) });
        }
    }
    outTerrainMesh.create(verts);

    outHasWater = false;
    for (int i = 0; i < CHUNK_RES * CHUNK_RES; i++) {
        if (positions[i].y < localWaterLevels[i] + 0.1f) {
            outHasWater = true;
            break;
        }
    }

    if (outHasWater) {
        std::vector<Vertex> waterVerts;
        glm::vec4 waterColor(0.15f, 0.35f, 0.55f, 0.8f);

        for (int z = 0; z < CHUNK_RES - 1; z++) {
            for (int x = 0; x < CHUNK_RES - 1; x++) {
                int idxTL = z * CHUNK_RES + x;
                int idxTR = z * CHUNK_RES + x + 1;
                int idxBL = (z + 1) * CHUNK_RES + x;
                int idxBR = (z + 1) * CHUNK_RES + x + 1;

                glm::vec3 pTL = positions[idxTL]; pTL.y = localWaterLevels[idxTL];
                glm::vec3 pTR = positions[idxTR]; pTR.y = localWaterLevels[idxTR];
                glm::vec3 pBL = positions[idxBL]; pBL.y = localWaterLevels[idxBL];
                glm::vec3 pBR = positions[idxBR]; pBR.y = localWaterLevels[idxBR];

                glm::vec3 n1 = glm::normalize(glm::cross(pTR - pTL, pBL - pTL));
                glm::vec3 n2 = glm::normalize(glm::cross(pBL - pTR, pBR - pTR));

                waterVerts.push_back({ pTL, n1, waterColor });
                waterVerts.push_back({ pTR, n1, waterColor });
                waterVerts.push_back({ pBL, n1, waterColor });

                waterVerts.push_back({ pTR, n2, waterColor });
                waterVerts.push_back({ pBR, n2, waterColor });
                waterVerts.push_back({ pBL, n2, waterColor });
            }
        }
        outWaterMesh.create(waterVerts);
    }
}