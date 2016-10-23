#include <iostream>

#include <ngl/NGLInit.h>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include <ngl/Random.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>

#include <time.h>

#include "common.hpp"
#include "visualiser.hpp"
#include "util.hpp"

visualiser::visualiser()
{
    m_lmb = false;
    m_mmb = false;

    m_tZoom = 5.0f;
    m_cZoom = 5.0f;

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
    SDL_GL_SetSwapInterval(0);

    ngl::NGLInit::instance();

    ngl::Random * rnd = ngl::Random::instance();
    rnd->setSeed( time(NULL) );

    createShaderProgram( "blinn", "MVPUVNVert", "blinnFrag" );

    /*m_framebuffer.initialise();
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

    m_framebuffer.unbind();*/

    glViewport(0, 0, m_w, m_h);

    m_cam = ngl::Camera(
                ngl::Vec3(0, 0, 1),
                ngl::Vec3(0, 0, 0),
                ngl::Vec3(0, 1, 0)
                );

    m_cam.setShape(
                60.0f,
                (float)m_w / (float)m_h,
                0.5f,
                2048.0f
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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    clear();
    swap();
    hide();
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

void visualiser::createShaderProgram(const std::string _name, const std::string _vert, const std::string _frag)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->createShaderProgram(_name);
    slib->attachShader(_vert, ngl::ShaderType::VERTEX);
    slib->attachShader(_frag, ngl::ShaderType::FRAGMENT);

    slib->loadShaderSource(_vert, "shaders/" + _vert + ".glsl");
    slib->loadShaderSource(_frag, "shaders/" + _frag + ".glsl");

    slib->compileShader(_vert);
    slib->compileShader(_frag);

    slib->attachShaderToProgram(_name, _vert);
    slib->attachShaderToProgram(_name, _frag);

    slib->linkProgramObject(_name);
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

void visualiser::drawSpheres()
{
    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use( "blinn" );

    /*m_trans.reset();
    //m_trans.setPosition( ngl::Vec3(0.0f, 0.0f, 0.0f) );
    loadMatricesToShader();

    prim->draw( "sphere" );*/

    for(auto &i : m_points)
    {
        //std::cout << "drawing sphere at " << i.m_x << ", " << i.m_y << ", " << i.m_z << '\n';
        //m_trans.reset();

        ngl::Vec3 pos = i + ngl::Vec3(0.0f, sin(g_TIME + (i.m_x * i.m_z) / 512.0f), 0.0f);

        m_trans.setPosition( pos );
        loadMatricesToShader();

        prim->draw( "sphere" );
    }
}

void visualiser::loadMatricesToShader()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    ngl::Mat4 camTrans;
    camTrans.translate(
                m_camCLook.m_x,
                m_camCLook.m_y,
                m_camCLook.m_z
                );

    ngl::Mat4 VP = m_cam.getVPMatrix();
    ngl::Mat3 normalMat = VP;
    normalMat.inverse();
    ngl::Mat4 MVP = m_trans.getMatrix() * camTrans * m_rot * VP;

    slib->setRegisteredUniform( "MVP", MVP );
    slib->setRegisteredUniform( "normalMat", normalMat );
}

void visualiser::mouseDown(SDL_Event _event)
{
    switch(_event.button.button)
    {
    case SDL_BUTTON_LEFT:
        m_mouseOrigin = getMousePos();
        m_mousePos = m_mouseOrigin;
        m_lmb = true;
        break;
    case SDL_BUTTON_MIDDLE:
        m_mouseOrigin = getMousePos();
        m_mousePos = m_mouseOrigin;
        m_mmb = true;
        break;
    default:
        break;
    }
}

void visualiser::mouseUp(SDL_Event _event)
{
    switch(_event.button.button)
    {
    case SDL_BUTTON_LEFT:
        m_mouseOrigin = getMousePos();
        m_mousePos = m_mouseOrigin;
        m_lmb = false;
        break;
    case SDL_BUTTON_MIDDLE:
        m_mouseOrigin = getMousePos();
        m_mousePos = m_mouseOrigin;
        m_mmb = false;
        break;
    default:
        break;
    }
}

void visualiser::mouseWheel(int _dir)
{
    m_tZoom += -6.0f * _dir;
}

void visualiser::setBufferLocation(GLuint _buffer, int _index, int _size)
{
    glEnableVertexAttribArray(_index);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer( 0, _size, GL_FLOAT, GL_FALSE, 0, 0 );
}

void visualiser::update()
{
    if( m_lmb )
    {
        m_mousePos = getMousePos();
        m_tMouseRotation += m_mousePos - m_mouseOrigin;

        m_mouseOrigin = m_mousePos;
    }
    else if( m_mmb )
    {
        std::cout << m_cam.getLook().m_x << ", " << m_cam.getLook().m_y << ", " << m_cam.getLook().m_z << '\n';
        m_mousePos = getMousePos();
        ngl::Vec2 diff = m_mousePos - m_mouseOrigin;

        ngl::Mat4 rot = m_rot;
        rot.inverse();
        ngl::Vec4 camPos = m_cam.getEye() * rot;
        ngl::Vec4 right;
        right.cross( camPos, ngl::Vec4(0.0f, 1.0f, 0.0f, 0.0f) );
        right.normalize();
        ngl::Vec4 up;
        up.cross( camPos, right );
        up.normalize();

        ngl::Vec4 add = (right * diff.m_x + up * diff.m_y);
        /*if(add.length() > 0.0f)
            add.normalize();*/

        m_camTLook += add;

        m_mouseOrigin = m_mousePos;
    }

    m_cMouseRotation += (m_tMouseRotation - m_cMouseRotation) / 16.0f;

    //std::cout << "Mouse down, " << m_mouseRotation.m_x << ", " << m_mouseRotation.m_y << '\n';

    //Do rotations
    ngl::Mat4 pitch;
    ngl::Mat4 yaw;

    pitch.rotateX( m_cMouseRotation.m_y );
    yaw.rotateY( m_cMouseRotation.m_x );

    m_rot = yaw * pitch;

    m_cZoom += (m_tZoom - m_cZoom) / 16.0f;
    ngl::Vec4 cpos = m_cam.getEye();
    cpos.normalize();

    m_cam.setEye( cpos * m_cZoom );

    m_camCLook += (m_camTLook - m_camCLook ) / 16.0f;
}

