#include "circle.hpp"
#include <cmath>


static float distanceBetween(const Point2D &a,const Point2D &b)
{
  float dx = a.x - b.x;
  float dy = a.y - b.y;

  return sqrtf(dx*dx + dy*dy);
}


bool Circle::contains(const Point2D &p)
{
  return distanceBetween(center,p)<=radius;
}
