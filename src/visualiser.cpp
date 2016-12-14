#include <iostream>

#include <algorithm>

#include <ngl/NGLInit.h>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include <ngl/Random.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>

#include <ngl/NGLStream.h>

#include <time.h>

#include "common.hpp"
#include "physicsvars.hpp"
#include "printer.hpp"
#include "shape.hpp"
#include "visualiser.hpp"
#include "util.hpp"

visualiser::visualiser(size_t _order) :
    m_timer(0.0f),
    m_lockedCamera(true),
    m_steadicam(false)
{
    m_order = _order;
    std::cout << "p0\n";
    m_lmb = false;
    m_mmb = false;
    m_rmb = false;

    m_light = false;

    m_fov = 35.0f;
    m_tfov = 35.0f;

    m_tZoom = 5.0f;
    m_cZoom = 0.0f;

    m_cameraShake = 0.0f;

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

    std::cout << "Screen dimensions " << m_w << ", " << m_h << " : " << SDL_GetNumVideoDisplays() << '\n';

    m_window = SDL_CreateWindow("mGen",
                                0, 0,
                                m_w, m_h,
																SDL_WINDOW_OPENGL );

    if(!m_window)
        errorExit("Unable to create window");

    //Setting up the GL attributes before they can be used
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_gl_context = SDL_GL_CreateContext( m_window );

    if(!m_gl_context)
        errorExit("Unable to create GL context");

    makeCurrent();
    SDL_GL_SetSwapInterval(0);

    std::cout << "Pre init\n";
    ngl::NGLInit::instance();
    std::cout << "Post init\n";

    ngl::Random * rnd = ngl::Random::instance();
    rnd->setSeed( time(NULL) );

    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    createShaderProgram( "blinn", "MVPUVNVert", "blinnFrag" );
    createShaderProgram( "bufferLight", "screenquadVert", "bufferLightFrag" );
    createShaderProgram( "bufferBokeh", "screenquadVert", "bufferBokehFrag" );
    createShaderProgram( "bufferFlare", "screenquadVert", "bufferFlareFrag" );
    slib->use("bufferFlare");
    slib->setRegisteredUniform("resolution", ngl::Vec2(m_w, m_h));
    slib->use("bufferBokeh");
    slib->setRegisteredUniform("bgl_dim", ngl::Vec2(m_w, m_h));

    m_framebuffer.initialise(m_w, m_h);

    m_framebuffer.addTexture( "diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
    m_framebuffer.addTexture( "normal", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1 );
    m_framebuffer.addTexture( "position", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT2 );
    m_framebuffer.addTexture( "radius", GL_RED, GL_R8, GL_COLOR_ATTACHMENT3 );
		m_framebuffer.addTexture("emissive", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT4);
    m_framebuffer.addDepthAttachment("depth");

    m_framebuffer.activeColourAttachments(
		{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4}
                );

    if(!m_framebuffer.checkComplete())
        errorExit( "Error! Framebuffer failed!\n" );

    m_framebuffer.unbind();

    m_DOFbuffer.initialise(m_w, m_h);
    m_DOFbuffer.addTexture( "diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
    m_DOFbuffer.addTexture( "depth", GL_RED, GL_R16F, GL_COLOR_ATTACHMENT1 );

    m_DOFbuffer.activeColourAttachments(
    {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}
                );

    if(!m_DOFbuffer.checkComplete())
        errorExit( "Error! Framebuffer failed!\n" );

    m_DOFbuffer.unbind();

    m_flareBuffer.initialise(m_w, m_h);
    m_flareBuffer.addTexture("diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0);

    m_flareBuffer.activeColourAttachments(
    {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}
                );

    if(!m_flareBuffer.checkComplete())
        errorExit( "Error! Framebuffer failed!\n" );

    m_flareBuffer.unbind();

    GLint v = 0;
    glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &v );

    //Create and reserve light buffer.
    m_lights.reserve(MAX_LIGHTS);
    glGenBuffers(1, &m_lightbuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_lightbuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(light) * MAX_LIGHTS, &m_lights[0].m_pos.m_x, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glViewport(0, 0, m_w, m_h);

    m_cam.setInitPos(ngl::Vec3(0, 0, 1));
    m_cam.setInitPivot(ngl::Vec3(0, 0, 0));

    m_cam.setFOV( m_fov );
    m_cam.setAspect( (float)m_w / (float)m_h );

    m_cam.calculateP();
    m_cam.calculate();
    /*m_cam = ngl::Camera(
                                                                                                                                ngl::Vec3(0, 0, 1),
                                                                                                                                ngl::Vec3(0, 0, 0),
                                                                                                                                ngl::Vec3(0, 1, 0)
                                                                                                                                );*/

    /*m_cam.setShape(
                                                                                                                                90.0f,
                                                                                                                                (float)m_w / (float)m_h,
                                                                                                                                0.5f,
                                                                                                                                2048.0f
                                                                                                                                );*/

    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();

    for(int i = 0; i < 4; ++i)
    {
        std::string name ("sphereLOD" + std::to_string( i ));
        prim->createSphere(
                    name,
                    1.0f,
                    pow(2.0f, i + 2)
                    );
        m_meshes.push_back( name );
    }
    std::reverse( m_meshes.begin(), m_meshes.end() );

    createVAO(
                "screenquad",
    {
                    ngl::Vec4(-1.0f, -1.0f, 0.0f, 1.0f),
                    ngl::Vec4(-1.0f, 1.0f, 0.0f, 1.0f),
                    ngl::Vec4(1.0f, 1.0f, 0.0f, 1.0f),
                    ngl::Vec4(1.0f, -1.0f, 0.0f, 1.0f)
                },
    {
                    ngl::Vec2(0.0, 0.0),
                    ngl::Vec2(0.0, 1.0),
                    ngl::Vec2(1.0, 1.0),
                    ngl::Vec2(1.0, 0.0)
                }
                );

    createVAO(
                "floor",
    {
                    ngl::Vec4(-1000.0f, -10.0f, -1000.0f, 1.0f),
                    ngl::Vec4(-1000.0f, -10.0f, 1000.0f, 1.0f),
                    ngl::Vec4(1000.0f, -10.0f, -1000.0f, 1.0f),
                    ngl::Vec4(1000.0f, -10.0f, 1000.0f, 1.0f)
                },
    {
                    ngl::Vec2(0.0, 0.0),
                    ngl::Vec2(0.0, 1.0),
                    ngl::Vec2(1.0, 1.0),
                    ngl::Vec2(1.0, 0.0)
                }
                );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    clear();
    swap();
    hide();
}

visualiser::~visualiser()
{
    SDL_DestroyWindow( m_window );
}

void visualiser::addPoint(const ngl::Vec3 &_vec, const std::vector<std::vector<note>> &_state, const float _mass)
{
    sphere node(_vec, _mass);
    node.setName(_state);

    m_nodes.push_back( node );
}

void visualiser::broadPhase(ngl::Vec3 _min, ngl::Vec3 _max, const std::vector<sphere *> &_nodes, unsigned short _lvl)
{
    std::vector<sphere *> outNodes;
    outNodes.reserve( _nodes.size() );
    unsigned short count = 0;

    for(auto &i : _nodes)
    {
        if(sphereAABBRoughIntersect(_min, _max, i->getPos(), i->getRadius()))
        {
            count++;
            outNodes.push_back( i );
        }
    }

    if(count <= 4 or _lvl > 5)
        m_partitions.push_back( outNodes );
    else
    {
        ngl::Vec3 dim = _max - _min;
        dim /= 2.0f;

        for(int i = 0; i < 2; ++i)
            for(int j = 0; j < 2; ++j)
                for(int k = 0; k < 2; ++k)
                {
                    ngl::Vec3 newMin = _min + ngl::Vec3( dim.m_x * i, dim.m_y * j, dim.m_z * k);
                    broadPhase(
                                newMin,
                                newMin + dim,
                                outNodes,
                                _lvl + 1
                                );
                }
    }
}

void visualiser::castRayGetNode()
{
    //Viewport coordinates
    ngl::Vec2 mouse = getMousePos( );

    int winx = 0;
    int winy = 0;
    SDL_GL_GetDrawableSize( m_window, &winx, &winy );

    //Normalise
    mouse.m_x /= winx;
    mouse.m_y /= winy;
    mouse *= 2.0f;
    mouse -= ngl::Vec2(1.0f, 1.0f);
    mouse.m_y *= -1.0f;

    //std::cout << "mouse pos is " << mouse << '\n';

    //Convert to homogenous clip coordinates, ie add z and w component.
    ngl::Vec4 ray = ngl::Vec4(
                mouse.m_x,
                mouse.m_y,
                -1.0f,
                1.0f
                );

    //Multiply by inverse projection matrix
    ray = m_cam.getP().inverse() * ray;
    //We need no longer consider the z and w components.
    ray.m_z = -1.0f;
    //We do not want to translate, ray represents a direction.
    ray.m_w = 0.0f;

    //Multiply by inverse view
    ray = m_cam.getV().inverse().transpose() * ray;

    //Create direction vector, normalise.
    ngl::Vec3 dir = ngl::Vec3(
                ray.m_x,
                ray.m_y,
                ray.m_z
                );
    dir.normalize();

    sphere * pt = nullptr;
    float bestDist = F_MAX;
    for(auto &i : m_nodes.m_objects)
    {
        if(raySphereIntersect(
                    m_cam.getPos(),
                    dir,
                    i.getPos(),
                    i.getRadius()
                    ))
        {
            float dist = (m_cam.getPos() - i.getPos()).lengthSquared();
            if(dist < bestDist)
            {
                bestDist = dist;
                pt = &i;
            }
        }
    }

    if(pt != nullptr)
    {
        printer pr;
        pt->addLuminance(4.0f);
        pt->addVel( dir );
        std::vector<slot> * cons = pt->getConnections();
        for(auto &con : *cons)
            m_nodes.getByID(con)->addLuminance( 0.5f );
        pr.message( "Node\n" );
        pr.message(std::to_string(pt->getTotalLuminance()));
    }
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

void visualiser::createVAO(const std::string &_id, std::vector<ngl::Vec4> _verts)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Generate a VBO
    GLuint vertBuffer = createBuffer4f( _verts );
    setBufferLocation( vertBuffer, 0, 3 );

    std::pair<std::string, GLuint> p (_id, vao );
    m_vaos.insert( p );
}

void visualiser::createVAO(const std::string &_id, std::vector<ngl::Vec4> _verts, std::vector<ngl::Vec2> _UVs)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Generate a VBO
    GLuint vertBuffer = createBuffer4f( _verts );
    setBufferLocation( vertBuffer, 0, 4 );

    GLuint uvBuffer = createBuffer2f( _UVs );
    setBufferLocation( uvBuffer, 1, 2 );

    std::pair<std::string, GLuint> p (_id, vao );
    m_vaos.insert( p );
}

void visualiser::drawSpheres()
{
    m_framebuffer.bind();
		m_framebuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4});
    clear();

    m_lights.clear();

    ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    slib->use( "blinn" );

    for(auto &i : m_nodes.m_objects)
    {
				slib->setRegisteredUniform("baseColour", ngl::Vec4( i.getColour() ));
				slib->setRegisteredUniform("luminance", i.getTotalLuminance());
        slib->setRegisteredUniform("radius", i.getRadius());
        //std::cout << "drawing sphere at " << i.m_x << ", " << i.m_y << ", " << i.m_z << '\n';
        //m_trans.reset();

        //float dist = (i.getPos() * m_V).m_z;

        ngl::Vec3 pos = i.getPos()/* + ngl::Vec3(0.0f, sin(g_TIME + (i.m_x * i.m_z) / 512.0f), 0.0f)*/;

        m_scale.scale( i.getRadius(), i.getRadius(), i.getRadius() );
        m_trans.translate( pos.m_x, pos.m_y, pos.m_z );
        loadMatricesToShader();

        //dist /= 32.0f;

        /*size_t index = static_cast<size_t>(std::floor(dist));
                                                                                                                                index = clamp(index, size_t(0), m_meshes.size() - 1);*/
        prim->draw( m_meshes[0] );
        //prim->draw( m_meshes[index] );

        if(i.getLuminance() > 0.05f)
        {
            ngl::Vec4 pos = i.getPos();
            pos.m_w = 1.0f;
            m_lights.push_back( {pos, i.getColour(), i.getTotalLuminance() * 0.5f} );
        }
    }

    if(m_light)
    {
        slib->setRegisteredUniform("baseColour", ngl::Vec4(0.5f, 0.5f, 1.0f, 1.0f));
        m_trans.translate( -m_camCLook.m_x, -m_camCLook.m_y, -m_camCLook.m_z );
        m_lights.push_back(
        {
                        -m_camCLook,
                        ngl::Vec3(1.0f, 1.0f, 1.0f),
                        8.0f
                    }
                    );
        loadMatricesToShader();

        prim->draw( m_meshes[0] );
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_lightbuffer);
    GLvoid * dat = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(dat, &m_lights[0].m_pos.m_x, sizeof(light) * std::min( m_lights.size(), static_cast<size_t>(MAX_LIGHTS)));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void visualiser::finalise()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    //Set light uniforms
    GLuint id = slib->getProgramID("bufferFlare");
    GLuint lightBlockIndex = glGetUniformBlockIndex( id, "lightBuffer" );
    GLuint index = 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, index, m_lightbuffer);
    glUniformBlockBinding(id, lightBlockIndex, index);

    id = slib->getProgramID("bufferLight");
    lightBlockIndex = glGetUniformBlockIndex( id, "lightBuffer" );
    index = 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, index, m_lightbuffer);
    glUniformBlockBinding(id, lightBlockIndex, index);

    m_framebuffer.unbind();

    m_DOFbuffer.bind();
    m_DOFbuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});
		glClearColor(0.005f, 0.005f, 0.01f, 1.0f);
		clear();

    //Draw lighting.
    slib->use("bufferLight");
    slib->setRegisteredUniform( "activeLights", static_cast<int>(std::min( m_lights.size(), static_cast<size_t>(MAX_LIGHTS) )) );

    glBindVertexArray(m_vaos["screenquad"]);

    m_framebuffer.bindTexture(id, "diffuse", "diffuse", 0);
    m_framebuffer.bindTexture(id, "normal", "normal", 1);
    m_framebuffer.bindTexture(id, "position", "position", 2);
    m_framebuffer.bindTexture(id, "radius", "radius", 3);
		m_framebuffer.bindTexture(id, "emissive", "emissive", 4);

    ngl::Vec3 camPos = m_cam.getPos(); //m_V * m_cam.getEye() - m_camCLook;
    slib->setRegisteredUniform( "camPos", camPos );

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    m_DOFbuffer.unbind();

    m_flareBuffer.bind();
    m_flareBuffer.activeColourAttachments( { GL_COLOR_ATTACHMENT0 });
		clear();

    //Post process
    slib->use("bufferBokeh");
    slib->setRegisteredUniform("focalDepth", (camPos - m_cam.getPivot()).length());

    glBindVertexArray(m_vaos["screenquad"]);

    id = slib->getProgramID("bufferBokeh");

    m_DOFbuffer.bindTexture(id, "diffuse", "bgl_RenderedTexture", 0);
    m_DOFbuffer.bindTexture(id, "depth", "bgl_DepthTexture", 1);

    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    m_flareBuffer.unbind();
		clear();

    slib->use("bufferFlare");
    slib->setRegisteredUniform("VP", m_cam.getV() * m_cam.getP());
    slib->setRegisteredUniform( "activeLights", static_cast<int>(std::min( m_lights.size(), static_cast<size_t>(MAX_LIGHTS) )) );

    id = slib->getProgramID("bufferFlare");

    m_flareBuffer.bindTexture(id, "diffuse", "diffuse", 0);

    loadMatricesToShader();
    glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);

    SDL_GL_SwapWindow(m_window);
}

void visualiser::loadMatricesToShader()
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();

    //ngl::Mat3 normalMat = MV;
    //normalMat.inverse();

    ngl::Mat4 M = m_scale * m_trans;
    ngl::Mat4 MVP = M * m_cam.getVP();

    slib->setRegisteredUniform( "M", M );
    slib->setRegisteredUniform( "MVP", MVP );
    //slib->setRegisteredUniform( "normalMat", normalMat );
}

void visualiser::mouseDown(SDL_Event _event)
{
    const unsigned char * keys = SDL_GetKeyboardState(NULL);

    m_mouseOrigin = getMousePos();
    m_mousePos = m_mouseOrigin;
    switch(_event.button.button)
    {
    case SDL_BUTTON_LEFT:
        if(keys[SDL_SCANCODE_LCTRL])
        {
            castRayGetNode();
        }
        else
        {
            m_lmb = true;
        }
        break;
    case SDL_BUTTON_MIDDLE:
        m_mmb = true;
        break;
    case SDL_BUTTON_RIGHT:
        m_rmb = true;
        break;
    default:
        break;
    }
}

void visualiser::mouseUp(SDL_Event _event)
{
    m_mouseOrigin = getMousePos();
    m_mousePos = m_mouseOrigin;
    switch(_event.button.button)
    {
    case SDL_BUTTON_LEFT:
        m_lmb = false;
        break;
    case SDL_BUTTON_MIDDLE:
        m_mmb = false;
        break;
    case SDL_BUTTON_RIGHT:
        m_rmb = false;
        break;
    default:
        break;
    }
}

void visualiser::mouseWheel(int _dir)
{
    m_tZoom = clamp( m_tZoom - 6.0f * _dir, 0.0f, F_MAX );
}

void visualiser::setBufferLocation(GLuint _buffer, int _index, int _size)
{
    glEnableVertexAttribArray(_index);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer( _index, _size, GL_FLOAT, GL_FALSE, 0, 0 );
}

void visualiser::narrowPhase()
{
    //std::cout << "Narrow phase start!\n";
    for(size_t i = 0; i < m_partitions.size(); ++i)
    {
        //std::cout << "Partition " << i << " of " << m_partitions.size() << ". Size : " << m_partitions[i].size() << '\n';
        for(size_t j = 0; j < m_partitions[i].size(); ++j)
        {
            auto a = m_partitions[i][j];
            for(size_t k = j; k < m_partitions[i].size(); ++k)
            {
                auto b = m_partitions[i][k];
                if(a == b)
                    continue;

                ngl::Vec3 normal = b->getPos() - a->getPos();

                float ar = a->getRadius();
                float br = b->getRadius();

                float dist = normal.lengthSquared();
                if( dist > sqr(ar + br) )
                    continue;

                dist = sqrt(dist);
                normal /= dist;

                float aim = a->getInvMass();
                float bim = b->getInvMass();
                float sumMass = aim + bim;

                float penetration = (ar + br) - dist;
                penetration = std::max(penetration - (ar + br) * g_BALL_PENETRATION_LENIENCY, 0.0f);
                penetration /= sumMass;
                penetration *= 0.2f;

                if(dist == 0.0f)
                {
                    penetration = ar;
                    normal = ngl::Vec3(0.0f, 1.0f, 0.0f);
                }

                ngl::Vec3 toMove = normal * penetration;

                a->addPos( -toMove * aim / sumMass );
                b->addPos( toMove * bim / sumMass );

                ngl::Vec3 rv = b->getVel() - a->getVel();
                float separation = rv.dot( normal );

                if(separation >= 0.0f) continue;

                float force = -(1.0f + g_COLLISION_ENERGY_CONSERVATION) * separation;
                force /= sumMass;

                ngl::Vec3 impulse = force * normal;

                /*a->setVel(ngl::Vec3(0,0,0));
                b->setVel(ngl::Vec3(0,0,0));*/
                a->addVel( -aim * impulse );
                b->addVel( bim * impulse );

                if(force > 0.05f)
                {
                    a->addLuminance(force * aim / sumMass * 0.01f);
                    b->addLuminance(force * bim / sumMass * 0.01f);
                }
            }
        }
    }
    //std::cout << "Narrow phase end!\n";
}

void visualiser::update(const float _dt)
{
    m_cam.clearTransforms();

    float interpDiv = 8.0f;
    if(m_steadicam)
        interpDiv = 64.0f;

    m_fov += (m_tfov - m_fov) / interpDiv;
    m_cam.setFOV(m_fov);
    m_cam.calculateP();

    ngl::Random * rand = ngl::Random::instance();
    m_camTLook += rand->getRandomNormalizedVec3() * m_cameraShake;

    if( m_lmb )
    {
        m_mousePos = getMousePos();
        m_tMouseRotation += m_mousePos - m_mouseOrigin;

        m_mouseOrigin = m_mousePos;
    }
    else if( m_mmb )
    {
        m_mousePos = getMousePos();
        ngl::Vec2 diff = m_mousePos - m_mouseOrigin;

        /*m_rot.transpose();
                                m_camTLook += m_rot.getRightVector() * diff.m_x / 8.0f;
                                m_camTLook -= m_rot.getUpVector() * diff.m_y / 8.0f;*/

        m_camTLook += m_cam.right() * diff.m_x / interpDiv;
        m_camTLook -= m_cam.up() * diff.m_y / interpDiv;

        m_mouseOrigin = m_mousePos;
    }
    else if(m_rmb)
    {
        m_mousePos = getMousePos();
        ngl::Vec2 diff = m_mousePos - m_mouseOrigin;

        m_camTLook += m_cam.back() * diff.m_y / interpDiv;

        m_mouseOrigin = m_mousePos;
    }

    m_cMouseRotation += (m_tMouseRotation - m_cMouseRotation) / (interpDiv * 2.0f);

    m_cZoom += (m_tZoom - m_cZoom) / (interpDiv * 2.0f);

    m_cam.setInitPos( ngl::Vec3(0.0, 0.0, 1.0) * m_cZoom );

    m_camCLook += (m_camTLook - m_camCLook ) / (interpDiv * 2.0f);


    if(!m_lockedCamera)
    {
        m_cam.moveWorld( m_camCLook );
        m_cam.rotateCamera( m_cMouseRotation.m_y, m_cMouseRotation.m_x, 0.0f );
    }
    else
    {
        ngl::Vec3 averagePos;
        for(auto &i : m_nodes.m_objects)
            averagePos += i.getPos();
        averagePos /= m_nodes.size();

        m_cam.setInitPos( ngl::Vec3(0.0, 0.0, 256 + 4.0f * sinf(m_timer.getTime()) + m_cameraShake + m_cZoom) );
        m_cam.moveWorld( -averagePos );
        m_cam.moveWorld( m_camCLook );
        m_cam.rotateCamera( 0.0f, -5.0f * m_timer.getTime(), 0.0f );
    }

    m_cam.calculate();

    //TEST
    for(auto &i : m_nodes.m_objects)
    {
        ngl::Vec3 origin = i.getPos();

        //Loop through connections.
        for(auto &id : (*i.getConnections()))
        {
            sphere * target = m_nodes.getByID( id );

            target->addInheritedLuminance( i.getTotalLuminance() * _dt * 0.25f );

            float sumRad = i.getRadius() + target->getRadius();
            float sumMass = i.getInvMass() + target->getInvMass();

            //Get distance.
            ngl::Vec3 dir = target->getPos() - origin;
            float dist = dir.lengthSquared();
            dist = std::max(dist, 0.0f);

            float mrad = sumRad * g_BALL_STICKINESS_RADIUS_MULTIPLIER;
            if(dist > mrad * mrad)
            {
                dist = sqrt(dist);
                dir /= pow(dist, g_GRAVITY_ATTENUATION);
                //Add forces to both current node and connection node (connections are one-way so we must do both here).
                i.addForce( dir * (i.getInvMass() / sumMass) );
            }
            else
                i.addForce( -i.getVel() * std::min( g_BALL_STICKINESS * (dist / mrad), 1.0f) );

            //std::cout << i.getVel() << '\n';
        }
        //std::cout << "origin post " << origin.m_x << ", " << origin.m_y << ", " << origin.m_z << "\n\n";
    }

    m_partitions.clear();

    std::vector<sphere *> initNodes;
    initNodes.reserve( m_nodes.size() );

    std::vector<ngl::Vec3> points;
    points.reserve( m_nodes.size() );

    for(auto &i : m_nodes.m_objects)
    {
        initNodes.push_back( &i );
        points.push_back( i.getPos() );
    }

    std::pair<ngl::Vec3, ngl::Vec3> initBox = lim( points );

    broadPhase( initBox.first, initBox.second, initNodes, 0 );
    narrowPhase();

    for(auto &i : m_nodes.m_objects)
    {
        //i.setVel( i.getVel() * (1.0f - g_AMBIENT_FRICTION) );
        i.addForce( -i.getVel() * g_AMBIENT_FRICTION * i.getRadius() );
        i.update( _dt );
    }

    m_timer.setCur();
    //std::cout << "TIME : " << m_timer.getTime() << '\n';
    std::vector<float> data = m_sampler.sampleAudio( m_timer.getTime(), 8192 );
    std::vector<float> averaged;
    averageVector( data, averaged, 2 );

    std::vector<note> state;
    float noteMuls;
    state.reserve( 4096 );
    //Create nodes based on averages.
    for(size_t i = 0; i < averaged.size(); ++i)
    {
        const int peakWidth = 128;
        //Min and max indexes to search for peak.
        int mindex = i - peakWidth / 2;
        int maxdex = i + peakWidth / 2;
        if(mindex < 0)
            maxdex += -mindex;
        if(maxdex > averaged.size())
            mindex += averaged.size() - maxdex;
        mindex = std::max(0, mindex);
        maxdex = std::min((int)averaged.size() , maxdex);

        //Get average around current index. Average of an average, I know. This probably isn't very nice to read.
        float averagedAverage = 0.0f;
        for(size_t j = mindex; j < maxdex; ++j)
        {
            averagedAverage += averaged[j];
        }
        averagedAverage /= maxdex - mindex;

        //If this is a non-duplicate peak, add a note to the state.
        if(averaged[i] > averagedAverage)
        {
            float freq = i * sampler::getSampleRate() / averaged.size();
            note closest = closestNote(freq);
            closest.m_position = 0;
            if(std::find(state.begin(), state.end(), closest) == state.end())
            {
                //std::cout << "Adding note " << closest.m_type << " " << closest.m_position << '\n';
                state.push_back( closest);
                noteMuls += averaged[i] - averagedAverage;
            }
        }
    }

    fifoQueue( &m_stateBuffer, state, m_order );

    for(auto &node : m_nodes.m_objects)
    {
        if(node.getName() == m_stateBuffer)
        {
            ngl::Random * rand = ngl::Random::instance();
            float force = (randFlt(12000.0f, 20000.0f)) * node.getInvMass() * noteMuls;
            node.addForce( rand->getRandomNormalizedVec3() * force );
            node.addLuminance(force / 128.0f);
            m_cameraShake += force / 20000.0f;
        }
    }

    m_cameraShake = std::min(m_cameraShake, 10.0f);
    m_cameraShake *= 0.9f;
}

void visualiser::sound(const std::string _path)
{
    m_sampler.load( _path );
    m_timer.setStart();
    Mix_PlayChannel(-1, m_sampler.get(), 0);
    m_timer.setCur();
    std::cout << "Post play time " << m_timer.getTime() << '\n';
    //SDL_Delay(100);
}
