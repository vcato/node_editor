#include <vector>
#include "viewportcoords.hpp"
#include "viewportcircle.hpp"
#include "viewportrect.hpp"
#include "color.hpp"


extern void drawLine(const ViewportPoint &p1,const ViewportPoint &p2);
extern void drawCircle(const ViewportCircle &circle);
extern void drawRect(const ViewportRect &arg);
extern void drawRoundedRect(const ViewportRect &arg);

extern void
  drawFilledRoundedRect(
    const ViewportRect &rect,const Color &color
  );

extern void
  drawPolygon(
    const std::vector<ViewportPoint> &vertices,
    const Color &color
  );
