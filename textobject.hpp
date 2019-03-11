#ifndef TEXTOBJECT_HPP_
#define TEXTOBJECT_HPP_

#include <string>
#include "point2d.hpp"
#include "viewportcoords.hpp"


struct DiagramCoordsTag;
using DiagramPoint = TaggedPoint2D<DiagramCoordsTag>;
using DiagramVector = TaggedVector2D<DiagramCoordsTag>;


struct DiagramTextObject {
  std::string text;
  DiagramPoint position;
};


struct ViewportTextObject {
  std::string text;
  ViewportPoint position;
};

#endif /* TEXTOBJECT_HPP_ */
