#include "qtsceneviewer.hpp"

#include <iostream>
#include <QMouseEvent>
#include "draw.hpp"
#include "viewportdraw.hpp"
#include "sceneviewerimpl.hpp"


using std::cerr;
using Frame = Scene::Frame;


QtSceneViewer::QtSceneViewer()
{
}


QtSceneViewer::~QtSceneViewer()
{
}


void QtSceneViewer::paintGL()
{
  begin2DDrawing(width(),height());

  const Scene *scene_ptr = scenePtr();

  if (!scene_ptr) return;

  const Scene &scene = *scene_ptr;

  auto drawRect =
    [](const Scene::Body &,const ViewportRect &rect){ ::drawRect(rect); };

  scene_viewer::forEachSceneBodyRect(scene,drawRect);
}


void QtSceneViewer::redrawScene()
{
  update();
}


void QtSceneViewer::mousePressEvent(QMouseEvent *event_ptr)
{
  assert(event_ptr);
  QMouseEvent &event = *event_ptr;

  ViewportPoint p =
    screenToViewportCoords2(event.x(),event.y(),width(),height());

  mousePressedAt(p);
}


void QtSceneViewer::mouseReleaseEvent(QMouseEvent *)
{
  mouseReleased();
}


void QtSceneViewer::mouseMoveEvent(QMouseEvent * event_ptr)
{
  assert(event_ptr);
  QMouseEvent &event = *event_ptr;

  ViewportPoint p =
    screenToViewportCoords2(event.x(),event.y(),width(),height());

  mouseMovedTo(p);
}
