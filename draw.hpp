#include <GL/gl.h>
#include "point2d.hpp"
#include "viewportcoords.hpp"


extern void drawLine(Point2D line_start,Point2D line_end);
extern void ortho2D(float viewport_width,float viewport_height);
extern void clearScreen();
extern void begin2DDrawing(GLsizei viewport_width,GLsizei viewport_height);

inline ViewportPoint
  screenToViewportCoords2(int x,int y,int /*width*/,int height)
{
  float float_x = x;
  float float_y = y;
  return ViewportPoint{float_x,height-float_y};
}
