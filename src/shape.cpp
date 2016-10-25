#include <ngl/Types.h>
#include "shape.hpp"

bool rayIntersectSphere(ngl::Vec3 _start, ngl::Vec3 _dir, ngl::Vec3 _sphere, float _radius)
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
