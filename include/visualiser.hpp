#ifndef VISUALISER_HPP
#define VISUALISER_HPP

#include <ngl/Camera.h>
#include <ngl/Transformation.h>

#include "slotmap.hpp"

#include <SDL2/SDL.h>

#include "framebuffer.hpp"

struct visualiserNodes
{
    slotmap< ngl::Vec3 > m_points;
    slotmap< std::string > m_strings;
    slotmap< float > m_masses;
};

class visualiser
{
public:
    visualiser();
    ~visualiser();
    void addPoint(const ngl::Vec3 &_vec, const std::string &_name);
    void clearPoints() {m_nodes.m_points.clear();}
    void clear() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);}
    void drawSpheres();
    void drawVAO(const std::string &_id);
    void hide() {SDL_HideWindow( m_window );}
    void makeCurrent() { SDL_GL_MakeCurrent(m_window, m_gl_context); }
    void mouseUp(SDL_Event _event);
    void mouseDown(SDL_Event _event);
    void mouseWheel(int _dir);
    void show() {SDL_ShowWindow( m_window );}
    void swap() {SDL_GL_SwapWindow(m_window);}
    void update();
private:
    GLuint createBuffer1f(std::vector<float> _vec);
    GLuint createBuffer2f(std::vector<ngl::Vec2> _vec);
    GLuint createBuffer3f(std::vector<ngl::Vec3> _vec);
    GLuint createBuffer4f(std::vector<ngl::Vec4> _vec);

    void createShaderProgram(const std::string _name, const std::string _vert, const std::string _frag);

    void createVAO(const std::string &_id, std::vector<ngl::Vec3> _verts);

    void loadMatricesToShader();

    void setBufferLocation(GLuint _buffer, int _index, int _size);

    ngl::Camera m_cam;
    ngl::Transformation m_camTrans;
    framebuffer m_framebuffer;
    SDL_GLContext m_gl_context;
    int m_h;
    bool m_lmb;
    bool m_mmb;
    //Store where the mouse used to be.
    ngl::Vec2 m_mouseOrigin;
    //Store where the mouse currently is.
    ngl::Vec2 m_mousePos;
    //Store the rotation of the world caused by the mouse.
    ngl::Vec2 m_tMouseRotation;
    ngl::Vec2 m_cMouseRotation;

    ngl::Vec3 m_camTLook;
    ngl::Vec3 m_camCLook;

    visualiserNodes m_nodes;

    ngl::Transformation m_trans;
    std::map<std::string, GLuint> m_vaos;
    int m_w;
    SDL_Window * m_window;
    ngl::Mat4 m_rot;

    float m_cZoom;
    float m_tZoom;
};

#endif
