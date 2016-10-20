#include <ngl/NGLInit.h>

#include "visualiser.hpp"
#include "util.hpp"

visualiser::visualiser()
{
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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ngl::NGLInit::instance();

    m_framebuffer.setWidth( m_w );
    m_framebuffer.setHeight( m_h );



    //Generate textures
    m_bufferBackgroundDiffuse = genTexture(m_w, m_h, GL_RGBA, GL_RGBA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_bufferLitNormal = genTexture(m_w, m_h, GL_RGBA, GL_RGBA16F);
    m_bufferLitPosition = genTexture(m_w, m_h, GL_RGBA, GL_RGBA16F);
    m_bufferLitDiffuse = genTexture(m_w, m_h, GL_RGBA, GL_RGBA);
    m_bufferEffectsDiffuse = genTexture(m_w, m_h, GL_RGBA, GL_RGBA);

    glGenRenderbuffers(1, &m_bufferBackgroundDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_bufferBackgroundDepth);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_w, m_h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_bufferBackgroundDepth);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferEffectsDiffuse, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_bufferLitDiffuse, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_bufferLitNormal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_bufferLitPosition, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_bufferBackgroundDiffuse, 0);

    useAttachments({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4});

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Error! Framebuffer failed!\n";
        exit(EXIT_FAILURE);
    }

    //Set up framebuffer
    glGenFramebuffers(1, &m_tinyFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_tinyFramebuffer);

    m_bufferDownscaledBackgroundDiffuse = genTexture(m_w / AMBIENT_RESOLUTION_DIVIDER, m_h / AMBIENT_RESOLUTION_DIVIDER, GL_RGBA, GL_RGBA);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferDownscaledBackgroundDiffuse, 0);

    useAttachments({GL_COLOR_ATTACHMENT0});

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Error! Framebuffer failed!\n";
        exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, m_w, m_h);

    m_view = ngl::lookAt(ngl::Vec3(0,0,1),
                         ngl::Vec3(0,0,0),
                         ngl::Vec3(0,1,0));


    float divz = 1 / g_ZOOM_LEVEL;
    m_project = ngl::ortho(
                -g_HALFWIN.m_x * divz + m_cameraShakeOffset.m_x,
                g_HALFWIN.m_x * divz + m_cameraShakeOffset.m_x,
                g_HALFWIN.m_y * divz + m_cameraShakeOffset.m_y,
                -g_HALFWIN.m_y * divz + m_cameraShakeOffset.m_y,
                -2048.0,
                2048.0
                );

    m_uiProject = ngl::ortho(
                0.0f,
                m_w,
                m_h,
                0.0f,
                -256.0,
                256.0
                );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenBuffers(1, &m_vertBuffer);
    glGenBuffers(1, &m_UVBuffer);
    glGenBuffers(1, &m_colourBuffer);
    glGenBuffers(1, &m_genericBuffer);

    m_vao = createVAO({ngl::Vec3(0.0f, 0.0f, 0.0f), ngl::Vec3(0.0f, 1.0f, 0.0f)});

    m_unit_square_vao = createVAO({
                                      ngl::Vec3(-0.5f, -0.5f, 0.5f),
                                      ngl::Vec3(-0.5f, 0.5f, 0.5f),
                                      ngl::Vec3(0.5f, 0.5f, 0.5f),
                                      ngl::Vec3(0.5f, -0.5f, 0.5f)
                                  },
    {
                                      ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f),
                                      ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f),
                                      ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f),
                                      ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f)
                                  },
    {
                                      ngl::Vec2(0.0, 0.0),
                                      ngl::Vec2(0.0, 1.0),
                                      ngl::Vec2(1.0, 1.0),
                                      ngl::Vec2(1.0, 0.0)
                                  }
                                  );

    m_spriteVAO = createVAO({
                                ngl::Vec3(-1.0f, -1.0f, 0.5f),
                                ngl::Vec3(-1.0f, 1.0f, 0.5f),
                                ngl::Vec3(1.0f, 1.0f, 0.5f),
                                ngl::Vec3(1.0f, -1.0f, 0.5f)
                            },
    {
                                ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f),
                                ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f),
                                ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f),
                                ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f)
                            },
    {
                                ngl::Vec2(0.0, 0.0),
                                ngl::Vec2(0.0, 1.0),
                                ngl::Vec2(1.0, 1.0),
                                ngl::Vec2(1.0, 0.0)
                            }
                            );

    std::cout << "creating screen quad!\n";
    m_screenQuadVAO = createVAO({
                                    ngl::Vec3(-1.0f, -1.0f, 0.5f),
                                    ngl::Vec3(-1.0f, 1.0f, 0.5f),
                                    ngl::Vec3(1.0f, 1.0f, 0.5f),
                                    ngl::Vec3(1.0f, -1.0f, 0.5f)
                                },
    {
                                    ngl::Vec2(-0.5f, -0.5f),
                                    ngl::Vec2(-0.5f, 0.5f),
                                    ngl::Vec2(0.5f, 0.5f),
                                    ngl::Vec2(0.5f, -0.5f)
                                }
                                );

    m_pointVAO = createVAO({ngl::Vec3(0.0f, 0.0f, 0.0f)});

    m_shield = new ngl::Obj(g_GRAPHICAL_RESOURCE_LOC + "models/shield.obj");
    m_shield->createVAO();

    resetLights();
    m_activeLights = 0;

    std::cout << "p1\n";
    m_noise512 = loadTexture(g_GRAPHICAL_RESOURCE_LOC + "textures/util/noise512RGB.png", GL_RGB);
    std::cout << "p2\n";

    finalise(0.0f, vec2());

    std::cout << "g_HALFWIN is " << g_HALFWIN.m_x << ", " << g_HALFWIN.m_y << std::endl;
    std::cout << "Resolution: " << g_WIN_WIDTH << " x " << g_WIN_HEIGHT << std::endl;

}
