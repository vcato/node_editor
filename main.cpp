#include <QApplication>
#include <QDialog>
#include <iostream>
#include "qtmainwindow.hpp"
#include "qtsceneviewer.hpp"
#include "world.hpp"
#include "worldwrapper.hpp"



namespace {
struct QtWorld : World {
  QtMainWindow &main_window;

  QtWorld(QtMainWindow &main_window_arg)
  : main_window(main_window_arg)
  {
  }

  virtual void createSceneWindow(Scene &scene)
  {
    QDialog &dialog = createWidget<QDialog>(main_window);
    QBoxLayout &layout = createLayout<QVBoxLayout>(dialog);
    QtSceneViewer &viewer = createWidget<QtSceneViewer>(layout);
    viewer.setScenePtr(&scene);
    dialog.show();
  }
};
}


int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  QtMainWindow main_window;
  QtWorld world(main_window);
  WorldWrapper world_wrapper(world);
  main_window.setWorldPtr(&world_wrapper);
  main_window.show();
  return app.exec();
}
