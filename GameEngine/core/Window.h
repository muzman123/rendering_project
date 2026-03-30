#pragma once
#include <string>
#include <SDL.h>
#include <glad/glad.h>

class Window {
public:
    bool init(const std::string& title, int width, int height);
    void swapBuffers();
    void shutdown();

    SDL_Window* getSDLWindow() { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getAspect() const { return (float)m_width / m_height; }

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    int m_width = 0;
    int m_height = 0;
};