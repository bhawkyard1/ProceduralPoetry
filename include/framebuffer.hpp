#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <ngl/Types.h>

#include <map>
#include <vector>

class framebuffer
{
public:
    void initialise();
    void activeColourAttachments(const std::vector<GLenum> _bufs);
    void addTexture(const std::string &_identifier, GLenum _format, GLenum _iformat, GLenum _attachment);
    void addTexture(const std::string &_identifier, int _w, int _h, GLenum _format, GLenum _iformat, GLenum _attachment);
    void addDepthAttachment(const std::string &_identifier);
    void addDepthAttachment(const std::string &_identifier, int _w, int _h);
    void bind();
    void bindTexture(const GLint _shaderID, const std::string &_tex, const char *_uniform, int _target);
    bool checkComplete();
    void setHeight(const int _h) {m_h = _h;}
    void setWidth(const int _w) {m_w = _w;}
    void unbind();
private:
    GLuint genTexture(int _width, int _height, GLint _format, GLint _internalFormat);
    //Keeps track of textures.
    std::map< std::string, GLuint > m_textures;
    //Keeps track of the colour attachments that each texture uses.
    std::map< std::string, GLenum > m_attachments;
    GLuint m_framebuffer;
    int m_h;
    GLenum m_maxColourTarget;
    int m_w;
};

#endif
