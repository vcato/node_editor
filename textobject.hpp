#ifndef TEXTOBJECT_HPP_
#define TEXTOBJECT_HPP_

#include <string>
#include "point2d.hpp"
#include "viewportcoords.hpp"


struct DiagramCoordsTag;
using DiagramCoords = TaggedPoint2D<DiagramCoordsTag>;


struct DiagramTextObject {
  std::string text;
  DiagramCoords position;
};


struct ViewportTextObject {
  std::string text;
  ViewportCoords position;
};

#endif /* TEXTOBJECT_HPP_ */
