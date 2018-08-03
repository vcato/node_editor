#include "point2d.hpp"

#include <iostream>


std::ostream& operator<<(std::ostream &stream,const Point2D &p)
{
  stream << "Point2D(" << p.x << "," << p.y << ")";
  return stream;
}
