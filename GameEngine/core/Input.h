#pragma once
#include <SDL.h>

class Input {
public:
    void update();

    bool isKeyDown(SDL_Scancode key) const;
    bool isQuitRequested() const { return m_quit; }

    // Mouse delta since last frame
    int getMouseDX() const { return m_mouseDX; }
    int getMouseDY() const { return m_mouseDY; }

    void setMouseLocked(bool locked);

private:
    const Uint8* m_keyState = nullptr;
    int m_mouseDX = 0;
    int m_mouseDY = 0;
    bool m_quit = false;
};