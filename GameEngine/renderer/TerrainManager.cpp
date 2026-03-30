#include "TerrainManager.h"
#include <cmath>
#include <cstdlib>

void WorldStreamer::update(const glm::vec3& cameraPos, const TerrainGenerator& generator) {
    int camChunkX = (int)std::floor(cameraPos.x / CHUNK_SIZE);
    int camChunkZ = (int)std::floor(cameraPos.z / CHUNK_SIZE);

    // TODO: Move chunk vertex generation to a background worker thread. 
    // Synchronous generation here causes main thread stalls on chunk borders.

    for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {
        for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
            ChunkCoord coord{camChunkX + dx, camChunkZ + dz};

            if (m_activeChunks.find(coord) == m_activeChunks.end()) {
                TerrainChunk chunkData;
                generator.generateChunk(chunkData.terrainMesh, chunkData.waterMesh, chunkData.hasWater, coord.x, coord.z);
                m_activeChunks[coord] = chunkData;
            }
        }
    }

    for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); ) {
        int cx = it->first.x;
        int cz = it->first.z;

        if (std::abs(cx - camChunkX) > VIEW_DISTANCE + 1 ||
            std::abs(cz - camChunkZ) > VIEW_DISTANCE + 1) {
            it->second.destroy();
            it = m_activeChunks.erase(it);
        } else {
            ++it;
        }
    }
}

void WorldStreamer::draw() const {
    for (const auto& pair : m_activeChunks) {
        pair.second.terrainMesh.draw();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (const auto& pair : m_activeChunks) {
        if (pair.second.hasWater) {
            pair.second.waterMesh.draw();
        }
    }
    
    glDisable(GL_BLEND);
}

void WorldStreamer::destroy() {
    for (auto& pair : m_activeChunks) {
        pair.second.destroy();
    }
    m_activeChunks.clear();
}