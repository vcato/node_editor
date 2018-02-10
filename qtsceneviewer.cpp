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
    drawLine(Point2D(100,100),Point2D(200,200));
    ignore(body);
  }
}


void QtSceneViewer::redrawScene()
{
  update();
}
