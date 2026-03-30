#include "Time.h"

void Time::init() {
    m_start = SDL_GetPerformanceCounter();
    m_last = m_start;
}

void Time::update() {
    Uint64 now = SDL_GetPerformanceCounter();
    m_delta = (float)(now - m_last) / SDL_GetPerformanceFrequency();
    m_last = now;

    // Clamp to prevent huge jumps (e.g., when debugging)
    if (m_delta > 0.1f) m_delta = 0.016f;
}

float Time::getElapsed() const {
    return (float)(SDL_GetPerformanceCounter() - m_start) / SDL_GetPerformanceFrequency();
}