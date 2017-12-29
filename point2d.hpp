#ifndef POINT2D_HPP_
#define POINT2D_HPP_

#include <iostream>

struct Point2D {
  float x,y;

  Point2D()
  : x(0), y(0)
  {
  }

  Point2D(float x_arg,float y_arg)
  : x(x_arg), y(y_arg)
  {
  }

  bool operator==(const Point2D &arg) const
  {
    return x==arg.x && y==arg.y;
  }

  bool operator!=(const Point2D &arg) const
  {
    return !operator==(arg);
  }
};


inline std::ostream& operator<<(std::ostream &stream,const Point2D &p)
{
  stream << "Point2D(" << p.x << "," << p.y << ")";
  return stream;
}


#endif /* POINT2D_HPP_ */
