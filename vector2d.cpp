#include "vector2d.hpp"

#include <iostream>

using std::ostream;


ostream& operator<<(ostream &stream,const Vector2D &v)
{
  stream << "Vector2D(" << v.x << "," << v.y << ")";
  return stream;
}
