#ifndef SIZE2D_HPP_
#define SIZE2D_HPP_


#include <cassert>
#include "vector2d.hpp"


template <typename Tag>
struct TaggedSize2D : TaggedVector2D<Tag> {
  using TaggedVector2D<Tag>::TaggedVector2D;

  TaggedSize2D(float x,float y)
  : TaggedVector2D<Tag>(x,y)
  {
    assert(x>=0);
    assert(y>=0);
  }
};


#endif /* SIZE2D_HPP_ */
