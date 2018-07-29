#include "scenewindow.hpp"

#include <iostream>

using std::string;
using std::cerr;


void SceneWindow::notifySceneChanged()
{
  viewer().notifySceneChanged();
  tree().notifySceneChanged();
}


void SceneWindow::setScenePtr(Scene *arg,const string &name)
{
  viewer().setScenePtr(arg);
  tree().setScenePtr(arg);
  assert(arg);
  setTitle(name);
}


void SceneWindow::notifyBodyAdded(const Scene::Body &body)
{
  tree().notifyBodyAdded(body);
  viewer().notifySceneChanged();
}


void SceneWindow::notifyRemovingBody(const Scene::Body &body)
{
  tree().notifyRemovingBody(body);
}
