#include "scenewindow.hpp"

using std::string;


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
