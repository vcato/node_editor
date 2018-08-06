#ifndef TEXTOBJECT_HPP_
#define TEXTOBJECT_HPP_

#include <string>
#include "point2d.hpp"


#if 0
using DiagramCoordsTag = void;
using ViewportCoordsTag = void;
#else
struct DiagramCoordsTag;
struct ViewportCoordsTag;
#endif


using DiagramCoords = TaggedPoint2D<DiagramCoordsTag>;
using ViewportCoords = TaggedPoint2D<ViewportCoordsTag>;


struct DiagramTextObject {
  std::string text;
  DiagramCoords position;
};


struct ViewportTextObject {
  std::string text;
  ViewportCoords position;
};

#endif /* TEXTOBJECT_HPP_ */
