#ifndef TEXTOBJECT_HPP_
#define TEXTOBJECT_HPP_

#include <string>
#include "point2d.hpp"


#define USE_DIAGRAM_COORDS_FOR_TEXT_OBJECT_POSITION 0


#if USE_DIAGRAM_COORDS_FOR_TEXT_OBJECT_POSITION
struct DiagramCoords : Point2D {
  using Point2D::Point2D;
};


inline DiagramCoords operator-(const DiagramCoords &a,const Vector2D &b)
{
  return DiagramCoords(a-b);
}
#else
using DiagramCoords = Point2D;
#endif


struct TextObject {
  std::string text;
#if USE_DIAGRAM_COORDS_FOR_TEXT_OBJECT_POSITION
  DiagramCoords position;
#else
  Point2D position;
#endif
};

#endif /* TEXTOBJECT_HPP_ */
