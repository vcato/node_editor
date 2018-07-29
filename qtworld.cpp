#include "qtworld.hpp"

#include <QDialog>
#include "qtscenewindow.hpp"
#include "qtwidget.hpp"


QtWorld::QtWorld(QtMainWindow &main_window_arg)
: main_window(main_window_arg)
{
}


SceneWindow& QtWorld::createSceneViewerWindow(SceneMember &)
{
  QtSceneWindow &window = createWidget<QtSceneWindow>(main_window);
  window.show();

  return window;
}
