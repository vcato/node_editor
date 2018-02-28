#include "qtsceneviewer.hpp"

#include <iostream>
#include "draw.hpp"

using std::cerr;
using Frame = Scene::Frame;


QtSceneViewer::QtSceneViewer()
{
}


QtSceneViewer::~QtSceneViewer()
{
}


static void
  drawBodies(
    const Scene::Bodies &bodies,
    const Point2D &parent_global_position,
    const Frame& frame
  )
{
  for (const Scene::Body &body : bodies) {
    float gx = parent_global_position.x;
    float gy = parent_global_position.y;
    float x1 = gx + body.position.x(frame);
    float y1 = gy + body.position.y(frame);
    Point2D global_position(x1,y1);
    float x2 = x1 + 10;
    float y2 = y1 + 10;
    Point2D p1(x1,y1);
    Point2D p2(x2,y1);
    Point2D p3(x2,y2);
    Point2D p4(x1,y2);
    drawLine(p1,p2);
    drawLine(p2,p3);
    drawLine(p3,p4);
    drawLine(p4,p1);
    ignore(body);
    drawBodies(body.children,global_position,frame);
  }
}


void QtSceneViewer::paintGL()
{
  begin2DDrawing(width(),height());

  const Scene *scene_ptr = scenePtr();

  if (!scene_ptr) return;

  const Scene &scene = *scene_ptr;

  Point2D parent_global_position(0,0);

  drawBodies(scene.bodies(),parent_global_position,scene.displayFrame());
}


void QtSceneViewer::redrawScene()
{
  update();
}
