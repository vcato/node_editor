#include "draw.hpp"

#include <GL/gl.h>
#include <GL/glu.h>


void ortho2D(float viewport_width,float viewport_height)
{
  GLdouble left = 0;
  GLdouble right = viewport_width;
  GLdouble bottom = 0;
  GLdouble top = viewport_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(left,right,bottom,top);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void drawLine(Point2D line_start,Point2D line_end)
{
  float vertices[][2] = {
    {line_start.x,line_start.y},
    {line_end.x,line_end.y},
  };

  float colors[][3] = {
    {1,1,1},
    {1,1,1}
  };

  glVertexPointer(/*size*/2,/*type*/GL_FLOAT,/*stride*/0,vertices);
  glColorPointer(/*size*/3,/*type*/GL_FLOAT,/*stride*/0,colors);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  {
    glColor3f(1,1,1);
    unsigned int line_indices[] = {0,1};
    GLenum mode = GL_LINES;
    GLsizei count = 2;
    GLenum type = GL_UNSIGNED_INT;

    glDrawElements(mode,count,type,line_indices);
  }

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}


void clearScreen()
{
  GLfloat red = 0;
  GLfloat green = 0;
  GLfloat blue = 0;
  GLfloat alpha = 1;

  glClearColor(red,green,blue,alpha);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void begin2DDrawing(GLsizei viewport_width,GLsizei viewport_height)
{
  GLint x = 0;
  GLint y = 0;
  glViewport(x,y,viewport_width,viewport_height);
  ortho2D(viewport_width,viewport_height);

  clearScreen();
}
