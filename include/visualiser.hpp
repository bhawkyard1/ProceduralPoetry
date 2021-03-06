#ifndef VISUALISER_HPP
#define VISUALISER_HPP

#include <SDL2/SDL.h>

#include <ngl/Camera.h>
#include <ngl/Transformation.h>
#include <ngl/Random.h>

#include "camera.hpp"
#include "framebuffer.hpp"
#include "light.hpp"
#include "slotmap.hpp"
#include "sphere.hpp"
#include "util.hpp"

#define MAX_LIGHTS 512

class visualiser
{
public:
    visualiser();
    ~visualiser();
    void addPoint(const ngl::Vec3 &_vec, const std::string &_name, const float _mass);
    void broadPhase(ngl::Vec3 _min, ngl::Vec3 _max, const std::vector<sphere *> &_nodes, unsigned short _lvl);
    void castRayGetNode();
    void clearPoints() {m_nodes.clear();}
    void clear() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);}
    slotmap<sphere> * getNodesPt() {return &m_nodes;}
    void drawSpheres();
    void drawVAO(const std::string &_id);
    void finalise();
    void hide() {SDL_HideWindow( m_window );}
    void makeCurrent() { SDL_GL_MakeCurrent(m_window, m_gl_context); }
    void mouseUp(SDL_Event _event);
    void mouseDown(SDL_Event _event);
    void mouseWheel(int _dir);
    void narrowPhase();
    void show() {SDL_ShowWindow( m_window );}
    void swap() {SDL_GL_SwapWindow(m_window);}
    void update(const float _dt);

    void resetPos()
    {
        ngl::Random * rnd = ngl::Random::instance();
        for(auto &i : m_nodes.m_objects)
        {
            i.setPos( rnd->getRandomNormalizedVec3() * randFlt(0.0f, 256.0f) );
            i.setVel( ngl::Vec3(0.0f, 0.0f, 0.0f) );
            i.setForces( ngl::Vec3(0.0f, 0.0f, 0.0f) );
        }
    }

    void toggleLight() {m_light = !m_light;}
private:
    GLuint createBuffer1f(std::vector<float> _vec);
    GLuint createBuffer2f(std::vector<ngl::Vec2> _vec);
    GLuint createBuffer3f(std::vector<ngl::Vec3> _vec);
    GLuint createBuffer4f(std::vector<ngl::Vec4> _vec);

    void createShaderProgram(const std::string _name, const std::string _vert, const std::string _frag);

    void createVAO(const std::string &_id, std::vector<ngl::Vec4> _verts);
    void createVAO(const std::string &_id, std::vector<ngl::Vec4> _verts, std::vector<ngl::Vec2> _UVs);

    void loadMatricesToShader();

    void setBufferLocation(GLuint _buffer, int _index, int _size);

    //ngl::Camera m_cam;
    camera m_cam;

    ngl::Transformation m_camTrans;
    framebuffer m_framebuffer;
    framebuffer m_DOFbuffer;
    std::vector<light> m_lights;
    GLuint m_lightbuffer;
    SDL_GLContext m_gl_context;
    int m_h;
    bool m_lmb;
    bool m_mmb;
    bool m_rmb;
    //Store where the mouse used to be.
    ngl::Vec2 m_mouseOrigin;
    //Store where the mouse currently is.
    ngl::Vec2 m_mousePos;
    //Store the rotation of the world caused by the mouse.
    ngl::Vec2 m_tMouseRotation;
    ngl::Vec2 m_cMouseRotation;

    ngl::Vec3 m_camTLook;
    ngl::Vec3 m_camCLook;

    slotmap<sphere> m_nodes;

    std::vector<std::vector<sphere *>> m_partitions;

    ngl::Mat4 m_scale;
    ngl::Mat4 m_trans;
    ngl::Mat4 m_V;
    ngl::Mat4 m_VP;
    std::unordered_map<std::string, GLuint> m_vaos;
    int m_w;
    SDL_Window * m_window;
    ngl::Mat4 m_rot;

    float m_cZoom;
    float m_tZoom;

    bool m_light;

    std::vector<std::string> m_meshes;
};

#endif
