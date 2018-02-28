#ifndef SCENEWINDOW_HPP_
#define SCENEWINDOW_HPP_

#include "sceneviewer.hpp"
#include "scenetree.hpp"


struct SceneWindow {
  virtual SceneViewer &viewer() = 0;
  virtual SceneTree &tree() = 0;

  void notifySceneChanged();
  void setScenePtr(Scene *);
  void setDisplayFrame(const Scene::Frame &);
};

#endif /* SCENEWINDOW_HPP_ */
