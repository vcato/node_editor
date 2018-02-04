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


static TreeItem motionPassItem()
{
  return TreeItem(world_policies::MotionPassPolicy{});
}


static TreeItem posExprItem()
{
  TreeItem pos_expr_item(world_policies::PosExprPolicy{});
  pos_expr_item.createItem2(world_policies::TargetBodyPolicy{});
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(world_policies::LocalPositionPolicy{});
    world_policies::createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(world_policies::GlobalPositionPolicy{});
    world_policies::createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }

  return pos_expr_item;
}


static TreeItem bodyItem()
{
  return TreeItem(world_policies::BodyPolicy{});
}


static TreeItem sceneItem()
{
  return TreeItem(world_policies::ScenePolicy{});
}


static TreeItem charmapperItem()
{
  return TreeItem(world_policies::CharmapperPolicy{});
}


namespace {
struct Charmapper {
  struct MotionPass {
  };

  vector<MotionPass> passes;

  void addMotionPass()
  {
    passes.push_back(MotionPass());
  }
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
    Charmapper charmapper;

    struct MotionPassWrapper {
      Charmapper::MotionPass &motion_pass;

      bool
        visitOperations(
          const TreePath &path,
          int depth,
          const OperationVisitor &visitor
        )
      {
        int path_length = path.size();

        if (depth==path_length) {
          visitor(
            "Add Pos Expr",
            [path](TreeOperationHandler &handler){
              handler.addItem(path,posExprItem());
            }
          );

          return true;
        }

        return false;
      }
    };

    virtual bool
      visitOperations(
        const TreePath &path,
        int depth,
        const OperationVisitor &visitor
      )
    {
      int path_length = path.size();

      if (depth==path_length) {
        visitor(
          "Add Motion Pass",
          [path,this](TreeOperationHandler &handler){
            charmapper.addMotionPass();
            handler.addItem(path,motionPassItem());
          }
        );

        return true;
      }

      int child_index = path[depth];

      MotionPassWrapper child_wrapper{charmapper.passes[child_index]};
      return child_wrapper.visitOperations(path,depth+1,visitor);
    }
  };

  struct SceneObject : WorldObject {
    virtual bool
      visitOperations(
        const TreePath &path,
        int depth,
        const OperationVisitor &visitor
      )
    {
      int path_length = path.size();

      if (depth==path_length) {
        visitor(
          "Add Body",
          [path](TreeOperationHandler &handler){
            handler.addItem(path,bodyItem());
          }
        );

        return true;
      }

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

  virtual bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    )
  {
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

      return true;
    }

    int child_index = path[depth];

    assert(world_objects[child_index]);
    WorldObject &child = *world_objects[child_index];
    return child.visitOperations(path,depth+1,visitor);
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
