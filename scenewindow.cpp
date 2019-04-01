#include "scenewindow.hpp"

#include <iostream>

using std::string;
using std::cerr;


void SceneWindow::notifySceneChanged()
{
  viewer().notifySceneChanged();
  tree().notifySceneChanged();
}


void
  SceneWindow::setScenePtr(
    Scene *scene_ptr,
    SceneListener *listener_ptr,
    const string &name
  )
{
  viewer().setScenePtr(scene_ptr,listener_ptr);
  tree().setScenePtr(scene_ptr);
  assert(scene_ptr);
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
