#include <ngl/Util.h>
#include <iostream>
#include "camera.hpp"

#include <ngl/NGLStream.h>

camera::camera()
{

}

void camera::calculate()
{
    m_cameraTransformation = ngl::Mat4();
    m_worldTransformation = ngl::Mat4();

    for(auto &i : m_cameraTransformationStack)
        m_cameraTransformation *= i;
    for(auto &i : m_worldTransformationStack)
        m_worldTransformation *= i;

    m_V = ngl::lookAt(
                m_initPos,
                m_initPivot,
                m_up
                );

    /*for(auto i = m_transformationStack.rbegin(); i < m_transformationStack.rend(); ++i)
        m_V *= (*i);*/

    m_V *= m_cameraTransformation;
    m_V *= m_worldTransformation;

    ngl::Mat4 VI = m_V.inverse();
    ngl::Mat4 wti = m_worldTransformation.inverse();
    m_pos = ngl::Vec3(VI.m_30, VI.m_31, VI.m_32);
    m_pivot = ngl::Vec3(wti.m_30, wti.m_31, wti.m_32);

    std::cout << "init pos " << m_initPos << '\n';
    std::cout << "pos " << m_pos << "\n\n";

    m_VP = m_V * m_P;
}

void camera::calculateP()
{
    m_P = ngl::perspective(
                m_fov,
                m_aspect,
                0.1f,
                10000.0f
                );
}

void camera::rotateCamera(const float _pitch, const float _yaw, const float _roll)
{
    m_cameraTransformationStack.push_back( rotationMatrix(_pitch, _yaw, _roll) );
}

void camera::rotateWorld(const float _pitch, const float _yaw, const float _roll)
{
    m_worldTransformationStack.push_back( rotationMatrix(_pitch, _yaw, _roll) );
}

void camera::moveCamera(const ngl::Vec3 _translation)
{
    m_cameraTransformationStack.push_back( translationMatrix(_translation) );
}

void camera::moveWorld(const ngl::Vec3 _translation)
{
    m_worldTransformationStack.push_back( translationMatrix(_translation) );
}

ngl::Mat4 camera::rotationMatrix(float _pitch, float _yaw, float _roll)
{
    ngl::Mat4 p;
    ngl::Mat4 y;
    ngl::Mat4 r;

    p.rotateX( _pitch );

    y.rotateY( _yaw );

    r.rotateZ( _roll );

    return y * p;
}

ngl::Mat4 camera::translationMatrix(const ngl::Vec3 &_vec)
{
    ngl::Mat4 trans;
    trans.translate( _vec.m_x, _vec.m_y, _vec.m_z );
    return trans;
}
