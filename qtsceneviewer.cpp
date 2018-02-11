#include "qtsceneviewer.hpp"

#include <iostream>
#include "draw.hpp"

using std::cerr;


QtSceneViewer::QtSceneViewer()
{
}


QtSceneViewer::~QtSceneViewer()
{
}


template <typename T>
static inline void ignore(const T&) { }


void QtSceneViewer::paintGL()
{
  begin2DDrawing(width(),height());

  const Scene *scene_ptr = scenePtr();

  if (!scene_ptr) return;

  for (const Scene::Body &body : scene_ptr->bodies()) {
    float x1 = body.position.x;
    float y1 = body.position.y;
    float x2 = body.position.x + 10;
    float y2 = body.position.y + 10;
    Point2D p1(x1,y1);
    Point2D p2(x2,y1);
    Point2D p3(x2,y2);
    Point2D p4(x1,y2);
    drawLine(p1,p2);
    drawLine(p2,p3);
    drawLine(p3,p4);
    drawLine(p4,p1);
    ignore(body);
  }
}


void QtSceneViewer::redrawScene()
{
  update();
}
