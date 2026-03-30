#pragma once
#include "Window.h"
#include "Input.h"
#include "Time.h"
#include "../renderer/Renderer.h"
#include "../scene/Scene.h"

class Engine {
public:
    bool init(const std::string& title, int width, int height);
    void run(Scene& scene);
    void shutdown(Scene& scene);

    Renderer& getRenderer() { return m_renderer; }
    Window& getWindow() { return m_window; }

private:
    Window m_window;
    Input m_input;
    Time m_time;
    Renderer m_renderer;
};