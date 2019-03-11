struct ViewportCoordsTag;

#include "size2d.hpp"


// Rename this to ViewportPoint
using ViewportPoint = TaggedPoint2D<ViewportCoordsTag>;
using ViewportVector = TaggedVector2D<ViewportCoordsTag>;
using ViewportSize = TaggedSize2D<ViewportCoordsTag>;
