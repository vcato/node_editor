#include "world.hpp"
#include "qtmainwindow.hpp"
#include "scenewindow.hpp"


struct QtWorld : World {
  QtMainWindow &main_window;

  QtWorld(QtMainWindow &main_window_arg);
  SceneWindow& createSceneViewerWindow(SceneMember &) override;
};
