#include <ngl/Util.h>
#include <iostream>
#include "camera.hpp"

#include <ngl/NGLStream.h>

camera::camera()
{

}

void camera::calculate()
{
	m_V = ngl::lookAt(
				m_initPos,
				m_initPivot,
				m_up
				);

    for(auto i = m_transformationStack.rbegin(); i < m_transformationStack.rend(); ++i)
		m_V *= (*i);

	ngl::Mat4 VI = m_V.inverse();
	m_pos = ngl::Vec4(m_initPos) * VI - m_initPos;
	m_pivot = ngl::Vec4(m_initPivot) * VI - m_initPos;

	std::cout << "init pivot " << m_initPivot << '\n';
	std::cout << "pivot " << m_pivot << "\n\n";

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

void camera::addRotation(const float _pitch, const float _yaw, const float _roll)
{
	ngl::Mat4 p;
	ngl::Mat4 y;
	ngl::Mat4 r;

	p.rotateX( _pitch );

	y.rotateY( _yaw );

	r.rotateZ( _roll );

	m_transformationStack.push_back( y * p );
}

void camera::addTranslation(const ngl::Vec3 _translation)
{
	ngl::Mat4 trans;
	trans.translate( _translation.m_x, _translation.m_y, _translation.m_z );

	m_transformationStack.push_back( trans );
}
