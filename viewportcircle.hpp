#ifndef VIEWPORTCIRCLE_HPP_
#define VIEWPORTCIRCLE_HPP_

#include "point2d.hpp"
#include "viewportcoords.hpp"


struct ViewportCircle {
  ViewportPoint center;
  float radius;

  bool contains(const ViewportPoint &p);
};


#endif /* VIEWPORTCIRCLE_HPP_ */
