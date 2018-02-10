#include "world.hpp"
#include "qtmainwindow.hpp"


struct QtWorld : World {
  QtMainWindow &main_window;

  QtWorld(QtMainWindow &main_window_arg);
  virtual SceneViewer& createSceneViewerWindow(SceneMember &);
};
