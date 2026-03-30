#pragma once
#include <SDL.h>

class Time {
public:
    void init();
    void update();

    float getDelta() const { return m_delta; }
    float getFPS() const { return 1.0f / m_delta; }
    float getElapsed() const;

private:
    Uint64 m_last = 0;
    Uint64 m_start = 0;
    float m_delta = 0.016f;
};