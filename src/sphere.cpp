#include "sphere.hpp"
#include "util.hpp"

sphere::sphere(ngl::Vec3 _pos, float _radius)
{
    m_pos = _pos;
    m_radius = _radius;
    m_invMass = 1.0f / _radius;
    m_vel = ngl::Vec3(0.0f, 0.0f, 0.0f);
    m_forces = ngl::Vec3(0.0f, 0.0f, 0.0f);
    m_colour = ngl::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_luminance = 0.0f;
    m_inheritedLuminance = 0.0f;
}

void sphere::update(float _dt)
{
    //_dt = clamp(_dt, 0.0f, 1.0f);

    m_vel += m_forces * _dt;
    m_forces.null();

    m_pos += m_vel * _dt;

    float mul = powf(0.9f, _dt + 1.0f);

		m_luminance = m_luminance * mul;
		m_inheritedLuminance = m_luminance * mul;

		m_luminance = std::max(m_luminance - _dt * 4.0f, 0.0f);
		m_inheritedLuminance = std::max(m_inheritedLuminance - _dt * 4.0f, 0.0f);
    /*m_luminance *= mul;
    m_inheritedLuminance = clamp(m_inheritedLuminance, 0.0f, 1.0f);
    m_inheritedLuminance *= mul;
        m_inheritedLuminance = 0.0f;*/
}
