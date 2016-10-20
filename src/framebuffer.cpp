#include <string>

#include "framebuffer.hpp"
#include "util.hpp"

framebuffer::framebuffer()
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &m_maxColourTarget );
    m_maxColourTarget += GL_COLOR_ATTACHMENT0;

    for(GLint i = GL_COLOR_ATTACHMENT0; i < m_maxColourTarget; ++i)
        m_colourAttachments.push_back( i );
}

void framebuffer::activeColourAttachments(const std::vector<GLenum> _bufs)
{
    for(auto &buf : _bufs)
    {
        if(buf > m_maxColourTarget)
            errorExit( "Error! Attempting to use colour target " + std::to_string((int)buf) + "that is not available on this system (max is " + std::to_string((int)m_maxColourTarget) + ").\n");
    }
    glDrawBuffers(_bufs.size(), &_bufs[0]);
}

void framebuffer::addTexture(const std::string &_identifier, GLenum _format, GLenum _iformat )
{
    addTexture(_identifier, m_w, m_h, _format, _iformat);
}

void framebuffer::addTexture(const std::string &_identifier, int _w, int _h, GLenum _format, GLenum _iformat )
{
    //Generate colour attachment.


    //Create texture.
    std::pair<std::string, GLuint> tex ( _identifier, genTexture(_w, _h, _format, _iformat) );
    m_textures.insert( tex );

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[ _identifier ], 0);
}

GLuint framebuffer::genTexture(int _width, int _height, GLint _format, GLint _internalFormat)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _format, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return tex;
}
