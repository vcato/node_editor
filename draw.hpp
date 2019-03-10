#include <GL/gl.h>
#include "point2d.hpp"
#include "viewportcoords.hpp"


extern void drawLine(Point2D line_start,Point2D line_end);
extern void ortho2D(float viewport_width,float viewport_height);
extern void clearScreen();
extern void begin2DDrawing(GLsizei viewport_width,GLsizei viewport_height);

