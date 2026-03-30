#pragma once
#include "Mesh.h"
#include "Terrain.h"
#include <glm/glm.hpp>
#include <unordered_map>

struct ChunkCoord {
    int x, z;
    
    bool operator==(const ChunkCoord& other) const {
        return x == other.x && z == other.z;
    }
};

// Specialized hash function for unordered_map lookup
namespace std {
    template <>
    struct hash<ChunkCoord> {
        std::size_t operator()(const ChunkCoord& k) const {
            return (std::hash<int>()(k.x) ^ (std::hash<int>()(k.z) << 1));
        }
    };
}

class TerrainChunk {
public:
    Mesh terrainMesh;
    Mesh waterMesh;
    bool hasWater = false;
    
    void destroy() {
        terrainMesh.destroy();
        if (hasWater) waterMesh.destroy();
    }
};

class WorldStreamer {
public:
    void update(const glm::vec3& cameraPos, const TerrainGenerator& generator);
    void draw() const;
    void destroy();

private:
    std::unordered_map<ChunkCoord, TerrainChunk> m_activeChunks;
    constexpr static int VIEW_DISTANCE = 3;
};