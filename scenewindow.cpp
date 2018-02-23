#include "scenewindow.hpp"


void SceneWindow::notifySceneChanged()
{
  viewer().notifySceneChanged();
  tree().notifySceneChanged();
}
