#include "Input.h"

void Input::update() {
    m_mouseDX = 0;
    m_mouseDY = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) m_quit = true;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            m_quit = true;
        if (event.type == SDL_MOUSEMOTION) {
            m_mouseDX += event.motion.xrel;
            m_mouseDY += event.motion.yrel;
        }
    }

    m_keyState = SDL_GetKeyboardState(nullptr);
}

bool Input::isKeyDown(SDL_Scancode key) const {
    return m_keyState && m_keyState[key];
}

void Input::setMouseLocked(bool locked) {
    SDL_SetRelativeMouseMode(locked ? SDL_TRUE : SDL_FALSE);
}