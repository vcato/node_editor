#include "scenewindow.hpp"


void SceneWindow::notifySceneChanged()
{
  viewer().notifySceneChanged();
  tree().notifySceneChanged();
}


void SceneWindow::setScenePtr(Scene *arg)
{
  viewer().setScenePtr(arg);
  tree().setScenePtr(arg);
}
