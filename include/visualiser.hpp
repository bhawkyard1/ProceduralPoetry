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
    void addPoint(const ngl::Vec3 &_vec) {m_points.push_back(_vec);}
    void drawVAO(const std::string &_id);
    void makeCurrent() const { SDL_GL_MakeCurrent(m_window, m_gl_context); }
private:
    GLuint createBuffer1f(std::vector<float> _vec);
    GLuint createBuffer2f(std::vector<ngl::Vec2> _vec);
    GLuint createBuffer3f(std::vector<ngl::Vec3> _vec);
    GLuint createBuffer4f(std::vector<ngl::Vec4> _vec);

    void createVAO(const std::string &_id, std::vector<ngl::Vec3> _verts);

    void loadMatricesToShader();

    void setBufferLocation(GLuint _buffer, int _index, int _size);

    ngl::Camera m_cam;
    framebuffer m_framebuffer;
    SDL_GLContext m_gl_context;
    int m_h;
    std::vector<ngl::Vec3> m_points;
    ngl::Transformation m_trans;
    std::map<std::string, GLuint> m_vaos;
    int m_w;
    SDL_Window * m_window;
};

#endif
