#include "circle.hpp"
#include <cmath>


static float distanceBetween(const ViewportCoords &a,const ViewportCoords &b)
{
  float dx = a.x - b.x;
  float dy = a.y - b.y;

  return sqrtf(dx*dx + dy*dy);
}


bool Circle::contains(const ViewportCoords &p)
{
  return distanceBetween(center,p)<=radius;
}
