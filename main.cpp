#include <QApplication>
#include <QDialog>
#include "qtmainwindow.hpp"
// #include "worldpolicies.hpp"


using std::vector;
using std::unique_ptr;
using std::make_unique;


namespace {
struct QtSceneViewer : QGLWidget {
  QSize sizeHint() const override { return QSize(640,480); }
};
}


namespace {
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

#if 0
  virtual void
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    )
  {
    using world_policies::charmapperItem;
    using world_policies::sceneItem;

    int path_length = path.size();

    if (depth==path_length) {
      visitor(
        "Add Charmapper",
        [path,this](TreeOperationHandler &handler){
          addCharmapper();
          handler.addItem(path,charmapperItem());
        }
      );
      visitor(
        "Add Scene",
        [path,this](TreeOperationHandler &handler){
          addScene();
          handler.addItem(path,sceneItem());
        }
      );
    }
  }
#else
  virtual void
    visitOperations(
      const TreePath &,int,const OperationVisitor &
    )
  {
    assert(false);
  }
#endif
};
}


int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  QtMainWindow main_window;
  QtWorld world(main_window);
  main_window.setWorldPtr(&world);
  main_window.show();
  return app.exec();
}
