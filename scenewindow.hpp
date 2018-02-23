#ifndef SCENEWINDOW_HPP_
#define SCENEWINDOW_HPP_

#include "sceneviewer.hpp"
#include "scenetree.hpp"


struct SceneWindow {
  virtual SceneViewer &viewer() = 0;
  virtual SceneTree &tree() = 0;

  void notifySceneChanged();
};

#endif /* SCENEWINDOW_HPP_ */
