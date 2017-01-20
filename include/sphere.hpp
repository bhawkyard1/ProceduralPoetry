//----------------------------------------------------------------------------------------------------------------------
/// \file sphere.hpp
/// \brief This file contains the sphere class, which represents the nodes in the visualisation. Main data is position,
/// velocity and 'luminance', a measure of how triggered the node is by the audio.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class sphere
/// \brief Contains physical attributes such as position and velocity, also luminance. Stored in the visualiser and
/// updated each frame.
//----------------------------------------------------------------------------------------------------------------------

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <string>
#include <vector>

#include <ngl/Vec3.h>
#include <ngl/Vec4.h>

#include "notes.hpp"
#include "slotmap.hpp"

class sphere
{
public:
    sphere(ngl::Vec3 _pos, float _radius);

    void addLuminance(const float _luminance) {m_luminance += _luminance;}
    void addInheritedLuminance(const float _luminance) {m_inheritedLuminance += _luminance;}
    void addConnection(const slot &_id) {m_connections.push_back( _id );}
    void addForce(const ngl::Vec3 &_force) {m_forces += _force;}
    void addPos(const ngl::Vec3 &_pos) {m_pos += _pos;}
    void addVel(const ngl::Vec3 &_vel) {m_vel += _vel;}

    void clearConnections() {m_connections.clear();}

    ngl::Vec3 getColour() const {return m_colour;}
    float getTotalLuminance() const {return m_luminance + m_inheritedLuminance;}
    float getLuminance() const {return m_luminance;}
    std::vector< slot > * getConnections() {return &m_connections;}
    slot getConnection(const size_t _i) {return m_connections[_i];}
    ngl::Vec3 getForces() const {return m_forces;}
    float getInvMass() const {return m_invMass;}
		std::vector<notes> getName() const {return m_name;}
    ngl::Vec3 getPos() const {return m_pos;}
    float getRadius() const {return m_radius + m_luminance / 8.0f;}
    ngl::Vec3 getVel() const {return m_vel;}

    void update(float _dt);

    void setLuminance(const float _luminance) {m_luminance = _luminance;}
    void setForces(const ngl::Vec3 &_forces) {m_forces = _forces;}
    void setInvMass(const float _invMass) {m_invMass = _invMass;}
		void setName(const std::vector<notes> &_name) {m_name = _name;}
    void setPos(const ngl::Vec3 &_pos) {m_pos = _pos;}
    void setRadius(const float _radius) {m_radius = _radius;}
    void setVel(const ngl::Vec3 &_vel) {m_vel = _vel;}
private:
    float m_luminance;
    float m_inheritedLuminance;
    std::vector< slot > m_connections;
    ngl::Vec3 m_forces;
    float m_invMass;
    ngl::Vec3 m_colour;
		std::vector<notes> m_name;
    ngl::Vec3 m_pos;
    float m_radius;
    ngl::Vec3 m_vel;
};

#endif
