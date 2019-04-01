#ifndef CIRCLE_HPP_
#define CIRCLE_HPP_

#include "point2d.hpp"
#include "viewportcoords.hpp"


struct Circle {
  ViewportPoint center;
  float radius;

  bool contains(const ViewportPoint &p);
};

#endif /* CIRCLE_HPP_ */
