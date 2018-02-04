#include <QApplication>
#include <QDialog>
#include <iostream>
#include "qtmainwindow.hpp"
#include "worldpolicies.hpp"
#include "world.hpp"



using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::cerr;


namespace {
struct QtSceneViewer : QGLWidget {
  QSize sizeHint() const override { return QSize(640,480); }
};
}


namespace {
struct QtWorld : World {
  QtMainWindow &main_window;

  QtWorld(QtMainWindow &main_window_arg)
  : main_window(main_window_arg)
  {
  }

  virtual void createSceneWindow()
  {
    QDialog &dialog = createWidget<QDialog>(main_window);
    QBoxLayout &layout = createLayout<QVBoxLayout>(dialog);
    createWidget<QtSceneViewer>(layout);
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
