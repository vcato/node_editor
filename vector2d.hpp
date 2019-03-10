#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include <iosfwd>
#include "euclideanpair.hpp"


template <typename Tag>
struct TaggedVector2D : EuclideanPair<float> {
  TaggedVector2D()
  : EuclideanPair<float>(0,0)
  {
  }

  template <typename Tag2>
  explicit TaggedVector2D(const TaggedVector2D<Tag2> &arg)
  : EuclideanPair<float>(arg)
  {
  }

  TaggedVector2D(float x_arg,float y_arg)
  : EuclideanPair<float>(x_arg,y_arg)
  {
  }

  bool operator==(const TaggedVector2D &arg) const
  {
    return x==arg.x && y==arg.y;
  }

  bool operator!=(const TaggedVector2D &arg) const
  {
    return !operator==(arg);
  }
};


using Vector2D = TaggedVector2D<void>;


template <typename Tag>
TaggedVector2D<Tag>
  operator+(const TaggedVector2D<Tag> &a,const TaggedVector2D<Tag> &b)
{
  return {a.x+b.x, a.y+b.y};
}


template <typename Tag>
inline TaggedVector2D<Tag> operator*(const TaggedVector2D<Tag> &a,float b)
{
  return {a.x*b, a.y*b};
}


inline Vector2D operator/(const Vector2D &a,float b)
{
  return Vector2D(a.x/b, a.y/b);
}


extern std::ostream& operator<<(std::ostream &,const Vector2D &v);


#endif /* VECTOR2D_HPP */
