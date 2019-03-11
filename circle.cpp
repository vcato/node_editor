#include "circle.hpp"
#include <cmath>


static float distanceBetween(const ViewportPoint &a,const ViewportPoint &b)
{
  float dx = a.x - b.x;
  float dy = a.y - b.y;

  return sqrtf(dx*dx + dy*dy);
}


bool Circle::contains(const ViewportPoint &p)
{
  return distanceBetween(center,p)<=radius;
}
