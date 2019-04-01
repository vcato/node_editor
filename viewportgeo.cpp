#include "viewportgeo.hpp"

#include <math.h>


std::vector<ViewportPoint> verticesOfRect(const ViewportRect &rect)
{
  std::vector<ViewportPoint> vertices;

  float x1 = rect.start.x;
  float y1 = rect.start.y;
  float x2 = rect.end.x;
  float y2 = rect.end.y;

  vertices.push_back(ViewportPoint{x1,y1});
  vertices.push_back(ViewportPoint{x2,y1});
  vertices.push_back(ViewportPoint{x2,y2});
  vertices.push_back(ViewportPoint{x1,y2});

  return vertices;
}


std::vector<ViewportPoint>
  roundedVerticesOf(const ViewportRect &rect,float offset)
{
  std::vector<ViewportPoint> vertices;
  float radius = 5;
  float v = radius*sqrtf(2)/2;

  float x1 = rect.start.x - offset;
  float x1a = x1 + radius;
  float x1b = x1a - v;
  float y1 = rect.start.y - offset;
  float y1a = y1 + radius;
  float y1b = y1a - v;
  float x2 = rect.end.x + offset;
  float x2a = x2 - radius;
  float x2b = x2a + v;
  float y2 = rect.end.y + offset;
  float y2a = y2 - radius;
  float y2b = y2a + v;

  vertices.push_back(ViewportPoint{x1,y1a});
  vertices.push_back(ViewportPoint{x1b,y1b});
  vertices.push_back(ViewportPoint{x1a,y1});

  vertices.push_back(ViewportPoint{x2a,y1});
  vertices.push_back(ViewportPoint{x2b,y1b});
  vertices.push_back(ViewportPoint{x2,y1a});

  vertices.push_back(ViewportPoint{x2,y2a});
  vertices.push_back(ViewportPoint{x2b,y2b});
  vertices.push_back(ViewportPoint{x2a,y2});

  vertices.push_back(ViewportPoint{x1a,y2});
  vertices.push_back(ViewportPoint{x1b,y2b});
  vertices.push_back(ViewportPoint{x1,y2a});

  return vertices;
}


std::vector<ViewportPoint> verticesOf(const ViewportCircle &circle)
{
  ViewportPoint center = circle.center;
  float radius = circle.radius;
  std::vector<ViewportPoint> vertices;

  for (int i=0; i!=10; ++i) {
    float fraction = i/10.0;
    float angle = 2*M_PI * fraction;
    float x = center.x + cos(angle)*radius;
    float y = center.y + sin(angle)*radius;
    vertices.push_back(ViewportPoint{x,y});
  }

  return vertices;
}
