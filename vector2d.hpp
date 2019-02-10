#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include <iosfwd>


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


inline Vector2D operator+(const Vector2D &a,const Vector2D &b)
{
  return Vector2D(a.x+b.x, a.y+b.y);
}


inline Vector2D operator*(const Vector2D &a,float b)
{
  return Vector2D(a.x*b, a.y*b);
}


inline Vector2D operator/(const Vector2D &a,float b)
{
  return Vector2D(a.x/b, a.y/b);
}


extern std::ostream& operator<<(std::ostream &,const Vector2D &v);


#endif /* VECTOR2D_HPP */
