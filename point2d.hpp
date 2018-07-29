#ifndef POINT2D_HPP_
#define POINT2D_HPP_

#include <iosfwd>

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


struct Vector2D {
  float x,y;

  Vector2D()
  : x(0), y(0)
  {
  }

  Vector2D(float x_arg,float y_arg)
  : x(x_arg), y(y_arg)
  {
  }

  bool operator==(const Vector2D &arg) const
  {
    return x==arg.x && y==arg.y;
  }

  bool operator!=(const Vector2D &arg) const
  {
    return !operator==(arg);
  }
};


inline Point2D &operator+=(Point2D &point,const Vector2D &vector)
{
  point.x += vector.x;
  point.y += vector.y;
  return point;
}


inline Point2D &operator-=(Point2D &point,const Vector2D &vector)
{
  point.x -= vector.x;
  point.y -= vector.y;
  return point;
}


inline Vector2D operator-(const Point2D &a,const Point2D &b)
{
  return Vector2D( a.x-b.x, a.y-b.y );
}


extern std::ostream& operator<<(std::ostream &stream,const Point2D &p);


#endif /* POINT2D_HPP_ */
