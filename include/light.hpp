//----------------------------------------------------------------------------------------------------------------------
/// \file light.hpp
/// \brief This class acts as a simple point light.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class light
/// \brief Contains vec4 position for transformation, a vec3 colour and a float opacity/brightness.
//----------------------------------------------------------------------------------------------------------------------

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
