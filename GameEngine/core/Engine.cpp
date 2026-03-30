#include "Engine.h"
#include <iostream>

bool Engine::init(const std::string& title, int width, int height) {
    if (!m_window.init(title, width, height)) return false;
    if (!m_renderer.init()) return false;
    m_time.init();
    m_input.setMouseLocked(true);
    return true;
}

void Engine::run(Scene& scene) {
    float aspect = m_window.getAspect();

    while (!m_input.isQuitRequested()) {
        m_time.update();
        m_input.update();

        float dt = m_time.getDelta();
        float elapsed = m_time.getElapsed();

        scene.camera.update(m_input, dt);

        TerrainGenerator& terrainGen = m_renderer.getTerrainGenerator();
        float groundHeight = terrainGen.getHeight(
            scene.camera.position.x, scene.camera.position.z
        );
        // NOTE: We enforce a minimum height to prevent the camera from clipping entirely through the mesh mesh.
        float eyeHeight = 2.0f; 
        if (scene.camera.position.y < groundHeight + eyeHeight) {
            scene.camera.position.y = groundHeight + eyeHeight;
        }

        m_renderer.getWorldStreamer().update(scene.camera.position, terrainGen);

        scene.update(dt, elapsed);

        m_renderer.render(scene, aspect);
        m_window.swapBuffers();
    }
}

void Engine::shutdown(Scene& scene) {
    scene.destroy();
    m_renderer.shutdown();
    m_window.shutdown();
}