#include "viewportdraw.hpp"

#include "draw.hpp"
#include "viewportgeo.hpp"


static void
  drawClosedLine(
    const std::vector<ViewportPoint> &vertices
  )
{
  int n = vertices.size();

  for (int i=0; i!=n; ++i) {
    drawLine(vertices[i],vertices[(i+1)%n]);
  }
}


void drawLine(const ViewportPoint &p1,const ViewportPoint &p2)
{
  drawLine(Point2D(p1),Point2D(p2));
}


void drawRoundedRect(const ViewportRect &arg)
{
  float offset = 0.5;
  drawClosedLine(roundedVerticesOf(arg,offset));
}


void drawRect(const ViewportRect &arg)
{
  drawClosedLine(verticesOfRect(arg));
}


void drawCircle(const ViewportCircle &circle)
{
  drawClosedLine(verticesOf(circle));
}


void
  drawPolygon(
    const std::vector<ViewportPoint> &vertices,
    const Color &color
  )
{
  int n_vertices = vertices.size();
  int vertex_size = 2;
  int color_size = 3;
  std::vector<float> vertex_data;
  vertex_data.resize(n_vertices*vertex_size);
  std::vector<float> color_data;
  color_data.resize(n_vertices*color_size);

  for (int i=0; i!=n_vertices; ++i) {
    vertex_data[i*vertex_size + 0] = vertices[i].x;
    vertex_data[i*vertex_size + 1] = vertices[i].y;

    color_data[i*color_size + 0] = color.r;
    color_data[i*color_size + 1] = color.g;
    color_data[i*color_size + 2] = color.b;
  }

  glVertexPointer(
    vertex_size,/*type*/GL_FLOAT,/*stride*/0,vertex_data.data()
  );
  glColorPointer(
    color_size,/*type*/GL_FLOAT,/*stride*/0,color_data.data()
  );

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  {
    std::vector<unsigned int> index_data(n_vertices);
    for (int i=0; i!=n_vertices; ++i) {
      index_data[i] = i;
    }
    GLsizei count = n_vertices;
    GLenum type = GL_UNSIGNED_INT;

    glDrawElements(GL_POLYGON,count,type,index_data.data());
  }

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}


void
  drawFilledRoundedRect(
    const ViewportRect &rect,const Color &color
  )
{
  float offset = 0;
  drawPolygon(roundedVerticesOf(rect,offset),color);
}
