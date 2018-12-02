#include "point2dobject.hpp"

#include <memory>


using std::make_unique;
using std::vector;


Any makePoint2DObject(const Point2D &point)
{
  return vector<Any>{point.x,point.y};
}
