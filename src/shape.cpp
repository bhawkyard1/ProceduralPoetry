#include <ngl/Types.h>
#include "shape.hpp"
#include "util.hpp"

bool raySphereIntersect(ngl::Vec3 _start, ngl::Vec3 _dir, ngl::Vec3 _sphere, float _radius)
{
  ngl::Vec3 cp = closest(_start, _dir, _sphere);
  ngl::Vec3 cd = cp - _sphere;

  return ((_radius * _radius) > cd.lengthSquared());
}

ngl::Vec3 closest(ngl::Vec3 p1, ngl::Vec3 d1, ngl::Vec3 p2)
{
    /*float m = d1.length();
    if(m == 0.0f) return p1;
    else m1 /= m;*/
    //return p1 + d1 * dotProd1( p2 - p1, m1 );
    return p1 + d1 * d1.dot( p2 - p1 );
}

bool sphereAABBRoughIntersect(ngl::Vec3 _min, ngl::Vec3 _max, ngl::Vec3 _center, float _radius)
{
		if(_min.m_x > _center.m_x + _radius)
			return false;
		else if(_min.m_y > _center.m_y + _radius)
			return false;
		else if(_min.m_z > _center.m_z + _radius)
			return false;
		else if(_max.m_x < _center.m_x - _radius)
			return false;
		else if(_max.m_y < _center.m_y - _radius)
			return false;
		else if(_max.m_z < _center.m_z - _radius)
			return false;
		return true;
}

bool sphereAABBIntersect(ngl::Vec3 _min, ngl::Vec3 _max, ngl::Vec3 _center, float _radius)
{
    ngl::Vec3 closest = ngl::Vec3(
                clamp(_center.m_x, _min.m_x, _max.m_x),
                clamp(_center.m_y, _min.m_y, _max.m_y),
                clamp(_center.m_z, _min.m_z, _max.m_z)
                );

    return ( (closest - _center).lengthSquared() < sqr(_radius) );
}
