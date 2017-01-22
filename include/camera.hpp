//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file contains work developed as a part of
// my Computing for Animation 2 project, but used
// here. This code should not be marked here.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//----------------------------------------------------------------------------------------------------------------------
/// \file camera.hpp
/// \brief This class acts as a simple camera, that the user can move around the scene.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class camera
/// \brief Contains a set of matrices representing a camera.
//----------------------------------------------------------------------------------------------------------------------

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <ngl/Mat4.h>
#include <ngl/Vec3.h>

#include <vector>

class camera
{
public:
	camera();

	void calculate();
	void calculateP();

	ngl::Mat4 getV() const {return m_V;}
	ngl::Mat4 getP() const {return m_P;}
	ngl::Mat4 getVP() const {return m_VP;}

	ngl::Vec3 getPos() const {return m_pos;}
	ngl::Vec3 getPivot() const {return m_pivot;}

	void setAspect(const float _aspect) {m_aspect = _aspect;}
	void setFOV(const float _fov) {m_fov = _fov;}

	void setInitPos(const ngl::Vec3 &_pos) {m_initPos = _pos;}
	void setInitPivot(const ngl::Vec3 &_pivot) {m_initPivot = _pivot;}
	void setUp(const ngl::Vec3 &_up) {m_up = _up;}

	void moveCamera(const ngl::Vec3 _translation);
	void moveWorld(const ngl::Vec3 _translation);
	void rotateCamera(const float _pitch, const float _yaw, const float _roll);
	void rotateWorld(const float _pitch, const float _yaw, const float _roll);
	void transformCamera(const ngl::Mat4 _trans) {m_cameraTransformationStack.push_back(_trans);}
	void transformWorld(const ngl::Mat4 _trans) {m_worldTransformationStack.push_back(_trans);}

	void clearTransforms() {m_cameraTransformationStack.clear(); m_worldTransformationStack.clear();}

	ngl::Vec3 back() {return ngl::Mat4(m_V).transpose().getBackVector();}
	ngl::Vec3 forwards() {return ngl::Mat4(m_V).transpose().getForwardVector();}
	ngl::Vec3 up() {return ngl::Mat4(m_V).transpose().getUpVector();}
	ngl::Vec3 right() {return ngl::Mat4(m_V).transpose().getRightVector();}

	void setNear(const float _near) {m_near = _near;}
	void setFar(const float _far) {m_far = _far;}
	float getNear() const {return m_near;}
	float getFar() const {return m_far;}
private:
	float m_fov;
	float m_aspect;
	float m_near;
	float m_far;

	ngl::Vec3 m_pos;
	ngl::Vec3 m_initPos;
	ngl::Vec3 m_pivot;
	ngl::Vec3 m_initPivot;

	std::vector<ngl::Mat4> m_cameraTransformationStack;
	ngl::Mat4 m_cameraTransformation;
	std::vector<ngl::Mat4> m_worldTransformationStack;
	ngl::Mat4 m_worldTransformation;

	ngl::Mat4 m_P;
	ngl::Mat4 m_V;
	ngl::Mat4 m_VP;

	ngl::Vec3 m_up = ngl::Vec3(0.0f, 1.0f, 0.0f);

	ngl::Mat4 rotationMatrix(float _pitch, float _yaw, float _roll);
	ngl::Mat4 translationMatrix(const ngl::Vec3 &_vec);
};

#endif
