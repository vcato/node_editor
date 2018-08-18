#include "point2dobject.hpp"


Object makePoint2DObject(Point2D &point)
{
  return Object(*new Point2DObjectData{point});
}


void Point2DObjectData::printOn(std::ostream &stream) const
{
  stream << point;
}
