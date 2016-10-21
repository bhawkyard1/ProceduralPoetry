#include <iostream>

#include <ngl/NGLInit.h>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include <ngl/VAOPrimitives.h>

#include "visualiser.hpp"
#include "util.hpp"

visualiser::visualiser()
{
    std::cout << "p1\n";
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        errorExit("SDL initialisation failed");

    SDL_DisplayMode best;
    best.w = 0;
    best.h = 0;

    for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
    {
        SDL_DisplayMode temp;
        SDL_GetCurrentDisplayMode(i, &temp);
        if(temp.w * temp.h > best.w * best.h)
            best = temp;
    }

    m_w = best.w;
    m_h = best.h;


    m_window = SDL_CreateWindow("mGen",
                                0, 0,
                                m_w, m_h,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS );

    if(!m_window)
        errorExit("Unable to create window");

    //Setting up the GL attributes before they can be used
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_gl_context = SDL_GL_CreateContext( m_window );

    if(!m_gl_context)
        errorExit("Unable to create GL context");

    makeCurrent();
    SDL_GL_SetSwapInterval(1);

    ngl::NGLInit::instance();

    m_framebuffer.initialise();
    m_framebuffer.setWidth( m_w );
    m_framebuffer.setHeight( m_h );

    m_framebuffer.addTexture( "diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
    m_framebuffer.addTexture( "normal", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1 );
    m_framebuffer.addTexture( "position", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT2 );
    m_framebuffer.addDepthAttachment("depth");

    m_framebuffer.activeColourAttachments(
    {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2}
                );

    if(!m_framebuffer.checkComplete())
        errorExit( "Error! Framebuffer failed!\n" );

    m_framebuffer.unbind();

    glViewport(0, 0, m_w, m_h);

    m_cam = ngl::Camera(
                ngl::Vec3(0,0,1),
                ngl::Vec3(0,0,0),
                ngl::Vec3(0,1,0)
                );

    m_cam.setShape(
                45.0f,
                m_w / (float)m_h,
                0.01f,
                10000.0f
                );

    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();
    prim->createSphere( "sphere", 1.0f, 32.0f );

    createVAO(
                "screenquad",
    {
                    ngl::Vec3(-1.0f, -1.0f, 0.5f),
                    ngl::Vec3(-1.0f, 1.0f, 0.5f),
                    ngl::Vec3(1.0f, 1.0f, 0.5f),
                    ngl::Vec3(1.0f, -1.0f, 0.5f)
                }
                );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint visualiser::createBuffer1f(std::vector<float> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * _vec.size(),
                 &_vec[0],
            GL_STATIC_DRAW
            );
    return buffer;
}

GLuint visualiser::createBuffer2f(std::vector<ngl::Vec2> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(ngl::Vec2) * _vec.size(),
                 &_vec[0].m_x,
            GL_STATIC_DRAW
            );
    return buffer;
}

GLuint visualiser::createBuffer3f(std::vector<ngl::Vec3> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(ngl::Vec3) * _vec.size(),
                 &_vec[0].m_x,
            GL_STATIC_DRAW
            );
    return buffer;
}

GLuint visualiser::createBuffer4f(std::vector<ngl::Vec4> _vec)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(ngl::Vec4) * _vec.size(),
                 &_vec[0].m_x,
            GL_STATIC_DRAW
            );
    return buffer;
}

void visualiser::createVAO(const std::string &_id, std::vector<ngl::Vec3> _verts)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Generate a VBO
    GLuint vertBuffer = createBuffer3f( _verts );
    setBufferLocation( vertBuffer, 0, 3 );

    std::pair<std::string, GLuint> p (_id, vao );
    m_vaos.insert( p );
}

void visualiser::loadMatricesToShader()
{
    ngl::Mat4 VP = m_cam.getVPMatrix();
    ngl::Mat3 normalMat = VP;
    normalMat.inverse();
}

void visualiser::setBufferLocation(GLuint _buffer, int _index, int _size)
{
    glEnableVertexAttribArray(_index);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer( 0, _size, GL_FLOAT, GL_FALSE, 0, 0 );
}


