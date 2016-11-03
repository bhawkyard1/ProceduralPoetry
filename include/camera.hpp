#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <ngl/Mat4.h>

class camera
{
public:
	camera();

private:
	ngl::Vec3 m_pos;

	float zoom;
	ngl::Mat4 m_P;
	ngl::Mat4 m_V;
	ngl::Mat4 m_VP;

	ngl::Mat4 m_rotation0;
	ngl::Mat4 m_translate1;
	ngl::Mat4 m_rotation2;
};

#endif
