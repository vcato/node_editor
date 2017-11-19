#ifndef CIRCLE_HPP_
#define CIRCLE_HPP_

#include "point2d.hpp"


struct Circle {
  Point2D center;
  float radius;

  bool contains(const Point2D &p);
};

#endif /* CIRCLE_HPP_ */
