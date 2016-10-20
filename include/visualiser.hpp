#ifndef VISUALISER_HPP
#define VISUALISER_HPP

#include <ngl/Camera.h>
#include <ngl/Transformation.h>

#include <SDL2/SDL.h>

#include "framebuffer.hpp"

class visualiser
{
public:
    visualiser();
    void makeCurrent() const { SDL_GL_MakeCurrent(m_window, m_gl_context); }
private:
    ngl::Camera m_cam;
    framebuffer m_framebuffer;
    SDL_GLContext m_gl_context;
    ngl::Transformation m_trans;
    int m_h;
    int m_w;
    SDL_Window * m_window;
};

#endif
