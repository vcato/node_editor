#include <QApplication>
#include <QDialog>
#include <iostream>
#include "qtmainwindow.hpp"
#include "worldpolicies.hpp"


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
struct QtWorld : WorldInterface {
  QtMainWindow &main_window;

  struct WorldObject {
    virtual bool
      visitOperations(
        const TreePath &path,int depth,const OperationVisitor &visitor
      ) = 0;
  };

  struct CharmapperObject : WorldObject {
    virtual bool
      visitOperations(
        const TreePath &path,
        int depth,
        const OperationVisitor &visitor
      )
    {
      int path_length = path.size();

      if (depth==path_length) {
        world_policies::CharmapperPolicy().visitOperations(path,visitor);
        return true;
      }

      return false;
    }
  };

  struct SceneObject : WorldObject {
    virtual bool
      visitOperations(
        const TreePath &/*path*/,
        int /*depth*/,
        const OperationVisitor &/*visitor*/
      )
    {
      return false;
    }
  };

  vector<unique_ptr<WorldObject>> world_objects;

  QtWorld(QtMainWindow &main_window_arg)
  : main_window(main_window_arg)
  {
  }

  virtual void addScene()
  {
    world_objects.push_back(make_unique<SceneObject>());
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
  virtual bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    )
  {
    int path_length = path.size();

    if (depth==path_length) {
      world_policies::CharmapperPolicy().visitOperations(path,visitor);
      return true;
    }
    else {
      assert(false);
    }
  }
#endif

  virtual bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    )
  {
    int path_length = path.size();

    if (depth==path_length) {
      world_policies::visitRootOperations(path,visitor,*this);
      return true;
    }

    int child_index = path[depth];

    assert(world_objects[child_index]);
    WorldObject &child = *world_objects[child_index];
    child.visitOperations(path,depth+1,visitor);

    return false;
  }
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
