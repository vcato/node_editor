#include "qtsceneviewer.hpp"

#include <iostream>

using std::cerr;


QtSceneViewer::QtSceneViewer()
: scene_observer(*this)
{
}


QtSceneViewer::~QtSceneViewer()
{
}


void QtSceneViewer::paintGL()
{
  cerr << "QtSceneViewer::paintGL()\n";
}


void QtSceneViewer::setScenePtr(Scene *arg)
{
  scene_observer.setScenePtr(arg);
}


void QtSceneViewer::sceneChanged()
{
  cerr << "QtSceneViewer::sceneChanged()\n";
}
