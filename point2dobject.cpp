#include "point2dobject.hpp"

#include <memory>


using std::make_unique;


Object makePoint2DObject(Point2D &point)
{
  return Object(make_unique<Point2DObjectData>(point));
}


void Point2DObjectData::printOn(std::ostream &stream) const
{
  stream << point;
}
