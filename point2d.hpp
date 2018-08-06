#ifndef POINT2D_HPP_
#define POINT2D_HPP_

#include <iosfwd>
#include "vector2d.hpp"


#define USE_BASIC_POINT2D 0


template <typename Tag> struct TaggedPoint2D;

template <>
struct TaggedPoint2D<void> {
  float x,y;

  TaggedPoint2D()
  : x(0), y(0)
  {
  }

  TaggedPoint2D(float x_arg,float y_arg)
  : x(x_arg), y(y_arg)
  {
  }

  bool operator==(const TaggedPoint2D &arg) const
  {
    return x==arg.x && y==arg.y;
  }

  bool operator!=(const TaggedPoint2D &arg) const
  {
    return !operator==(arg);
  }
};


template <typename Tag>
struct TaggedPoint2D : TaggedPoint2D<void> {
  using TaggedPoint2D<void>::TaggedPoint2D;

  TaggedPoint2D() = default;

  explicit TaggedPoint2D(const TaggedPoint2D<void> &arg)
  : TaggedPoint2D<void>(arg)
  {
  }
};


using Point2D = TaggedPoint2D<void>;


template <typename Tag>
inline TaggedPoint2D<Tag> &
  operator+=(TaggedPoint2D<Tag> &point,const Vector2D &vector)
{
  point.x += vector.x;
  point.y += vector.y;
  return point;
}


template <typename Tag>
inline TaggedPoint2D<Tag> &
  operator-=(TaggedPoint2D<Tag> &point,const Vector2D &vector)
{
  point.x -= vector.x;
  point.y -= vector.y;
  return point;
}


template <typename Tag>
inline Vector2D
  operator-(const TaggedPoint2D<Tag> &a,const TaggedPoint2D<Tag> &b)
{
  return Vector2D( a.x-b.x, a.y-b.y );
}


template <typename Tag>
inline TaggedPoint2D<Tag>
  operator-(const TaggedPoint2D<Tag> &a,const Vector2D &b)
{
  float x = a.x - b.x;
  float y = a.y - b.y;
  return {x,y};
}


template <typename Tag>
inline TaggedPoint2D<Tag>
  operator+(const TaggedPoint2D<Tag> &a,const Vector2D &b)
{
  float x = a.x + b.x;
  float y = a.y + b.y;
  return {x,y};
}


extern std::ostream& operator<<(std::ostream &stream,const Point2D &p);


#endif /* POINT2D_HPP_ */
