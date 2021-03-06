#include <vector>
#include "viewportcoords.hpp"
#include "viewportrect.hpp"
#include "viewportcircle.hpp"

extern std::vector<ViewportPoint>
  verticesOfRect(const ViewportRect &rect);

extern std::vector<ViewportPoint>
  roundedVerticesOf(const ViewportRect &rect,float offset);

extern std::vector<ViewportPoint> verticesOf(const ViewportCircle &circle);

