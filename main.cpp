#include <QApplication>
#include <QDialog>
#include "qtmainwindow.hpp"


using std::vector;
using std::unique_ptr;
using std::make_unique;


struct QtSceneViewer : QGLWidget {
  QSize sizeHint() const override { return QSize(640,480); }
};


struct QtWorld : WorldInterface {
  QtMainWindow &main_window;

  struct WorldObject {
  };

  struct CharmapperObject : WorldObject {
  };

  vector<unique_ptr<WorldObject>> world_objects;

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

  virtual void addCharmapper()
  {
    world_objects.push_back(make_unique<CharmapperObject>());
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
