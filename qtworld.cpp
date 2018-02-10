#include "qtworld.hpp"

#include <QDialog>
#include "qtsceneviewer.hpp"


QtWorld::QtWorld(QtMainWindow &main_window_arg)
: main_window(main_window_arg)
{
}


SceneViewer& QtWorld::createSceneViewerWindow(SceneMember &member)
{
  QDialog &dialog = createWidget<QDialog>(main_window);
  QBoxLayout &layout = createLayout<QVBoxLayout>(dialog);
  QtSceneViewer &viewer = createWidget<QtSceneViewer>(layout);
  viewer.setScenePtr(&member.scene);

  dialog.show();
  return viewer;
}
