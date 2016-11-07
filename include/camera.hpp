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

	void addTranslation(const ngl::Vec3 _translation);
	void addRotation(const float _pitch, const float _yaw, const float _roll);
	void addTransformation(const ngl::Mat4 _trans) {m_transformationStack.push_back(_trans);}

	void clearTransforms() {m_transformationStack.clear();}

	ngl::Vec3 up() {return m_V.getUpVector();}
	ngl::Vec3 right() {return m_V.transpose().getRightVector();}
private:
	float m_fov;
	float m_aspect;

	ngl::Vec3 m_pos;
	ngl::Vec3 m_initPos;
	ngl::Vec3 m_pivot;
	ngl::Vec3 m_initPivot;

	std::vector<ngl::Mat4> m_transformationStack;

	ngl::Mat4 m_P;
	ngl::Mat4 m_V;
	ngl::Mat4 m_VP;	

	ngl::Vec3 m_up = ngl::Vec3(0.0f, 1.0f, 0.0f);
};

#endif
