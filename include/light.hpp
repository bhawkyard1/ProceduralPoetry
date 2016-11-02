#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <ngl/Vec3.h>

struct light
{
    ngl::Vec4 m_pos;
    ngl::Vec3 m_col;
    float m_brightness;
};

#endif // LIGHT_HPP
