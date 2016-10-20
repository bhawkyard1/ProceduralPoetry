#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <ngl/Types.h>

#include <vector>


class framebuffer
{
public:
    framebuffer();
    void activeColourAttachments(const std::vector<GLenum> _bufs);
    void addColourAttachment(GLenum _format, GLenum _iformat);
    void addColourAttachment(int _w, int _h, GLenum _format, GLenum _iformat);
    void setHeight(const int _h) {m_h = _h;}
    void setWidth(const int _w) {m_w = _w;}
private:
    GLuint genTexture(int _width, int _height, GLint _format, GLint _internalFormat);
    std::vector< GLuint > m_colourTargets;
    GLuint m_framebuffer;
    int m_h;
    GLint m_maxColourTarget;
    int m_w;
};

#endif
