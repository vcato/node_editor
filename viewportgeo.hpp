#include <vector>
#include "viewportcoords.hpp"
#include "viewportrect.hpp"
#include "circle.hpp"

extern std::vector<ViewportPoint>
  verticesOfRect(const ViewportRect &rect);

extern std::vector<ViewportPoint>
  roundedVerticesOf(const ViewportRect &rect,float offset);

extern std::vector<ViewportPoint> verticesOf(const Circle &circle);

