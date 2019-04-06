#ifndef POINT2D_HPP_
#define POINT2D_HPP_

#include <iostream>
#include "vector2d.hpp"

template <typename Tag> struct TaggedPoint2D;

template <typename Tag>
struct TaggedPoint2D : EuclideanPair<float> {
  TaggedPoint2D()
  : EuclideanPair<float>(0,0)
  {
  }

  template <typename Tag2>
  explicit TaggedPoint2D(const TaggedPoint2D<Tag2> &arg)
  : EuclideanPair<float>(arg)
  {
  }

  TaggedPoint2D(float x_arg,float y_arg)
  : EuclideanPair<float>(x_arg,y_arg)
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


// In General, two coordinate system tags do not have a common
// coordinate system.
template <typename Tag1,typename Tag2>
struct CommonCoordinateSystem {};


// If the coordinate systems are the same, then the common coordinate
// system is the one coordinate system.
template <typename Tag>
struct CommonCoordinateSystem<Tag,Tag> {
  using type = Tag;
};


// This is needed to get around ambiguity.
template <>
struct CommonCoordinateSystem<void,void> {
  using type = void;
};


// Every coordinate system is also considered to be in the void
// coordinate system.
template <typename Tag>
struct CommonCoordinateSystem<Tag,void> {
  using type = void;
};


// Every coordinate system is also considered to be in the void
// coordinate system.
template <typename Tag>
struct CommonCoordinateSystem<void,Tag> {
  using type = void;
};


template <typename Tag1,typename Tag2>
using CommonCoordinateSystem_t =
  typename CommonCoordinateSystem<Tag1,Tag2>::type;


template <typename Tag>
inline TaggedVector2D<Tag>
  operator-(const TaggedPoint2D<Tag> &a,const TaggedPoint2D<Tag> &b)
{
  return { a.x-b.x, a.y-b.y };
}


template <typename Tag>
inline TaggedPoint2D<Tag>
  operator-(const TaggedPoint2D<Tag> &a,const TaggedVector2D<Tag> &b)
{
  float x = a.x - b.x;
  float y = a.y - b.y;
  return {x,y};
}


template <typename Tag1,typename Tag2>
TaggedPoint2D<CommonCoordinateSystem_t<Tag1,Tag2>>
  operator+(const TaggedPoint2D<Tag1> &a,const TaggedVector2D<Tag2> &b)
{
  float x = a.x + b.x;
  float y = a.y + b.y;
  return {x,y};
}


template <typename Tag>
std::ostream&
  operator<<(std::ostream &stream,const TaggedPoint2D<Tag> &p)
{
  stream << "(" << p.x << "," << p.y << ")";
  return stream;
}


#endif /* POINT2D_HPP_ */
