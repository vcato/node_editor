#include <QApplication>
#include <QDialog>
#include "qtmainwindow.hpp"


struct QtSceneViewer : QGLWidget {
  QSize sizeHint() const override { return QSize(640,480); }
};


struct QtWorld : WorldInterface {
  QtMainWindow &main_window;

  QtWorld(QtMainWindow &main_window_arg)
  : main_window(main_window_arg)
  {
  }

  virtual void addScene()
  {
    QDialog &dialog = createWidget<QDialog>(main_window);
    QBoxLayout &layout = createLayout<QVBoxLayout>(dialog);
    createWidget<QtSceneViewer>(layout);
    dialog.show();
  }
};


int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  QtMainWindow main_window;
  QtWorld world(main_window);
  main_window.setWorldPtr(&world);
  main_window.show();
  return app.exec();
}
