#include "sphere.hpp"

sphere::sphere(ngl::Vec3 _pos, float _radius)
{
    m_pos = _pos;
    m_radius = _radius;
    m_invMass = 1.0f / _radius;
    m_vel = ngl::Vec3(0.0f, 0.0f, 0.0f);
    m_forces = ngl::Vec3(0.0f, 0.0f, 0.0f);
    m_colour = ngl::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_luminance = 0.0f;
}

void sphere::update(const float _dt)
{
    m_vel += m_forces * _dt;
    m_forces.null();

    m_pos += m_vel * _dt;

    m_luminance *= 1.0f - _dt * 0.1f;
}
