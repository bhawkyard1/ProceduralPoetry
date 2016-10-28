#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <ngl/Vec3.h>

ngl::Vec3 closest(ngl::Vec3 p1, ngl::Vec3 d1, ngl::Vec3 p2);

bool raySphereIntersect(ngl::Vec3 _start, ngl::Vec3 _dir, ngl::Vec3 _sphere, float _radius);

#endif
