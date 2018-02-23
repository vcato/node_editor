#include "qtworld.hpp"

#include <QDialog>
#include "qtscenewindow.hpp"


QtWorld::QtWorld(QtMainWindow &main_window_arg)
: main_window(main_window_arg)
{
}


SceneWindow& QtWorld::createSceneViewerWindow(SceneMember &member)
{
  QtSceneWindow &window = createWidget<QtSceneWindow>(main_window);
  window.setScenePtr(&member.scene);
  window.show();

  return window;
}
