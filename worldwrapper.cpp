#include "worldwrapper.hpp"

#include "worldpolicies.hpp"


using OperationVisitor = TreeItem::OperationVisitor;


static TreeItem charmapperItem()
{
  return TreeItem(world_policies::CharmapperPolicy{});
}


static TreeItem motionPassItem()
{
  return TreeItem(world_policies::MotionPassPolicy{});
}


static TreeItem sceneItem()
{
  return TreeItem(world_policies::ScenePolicy{});
}


static TreeItem bodyItem()
{
  return TreeItem(world_policies::BodyPolicy{});
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


namespace {
struct CharmapperWrapper {
  Charmapper &charmapper;

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

  bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    )
  {
    int path_length = path.size();

    if (depth==path_length) {
      Charmapper &charmapper = this->charmapper;
      visitor(
        "Add Motion Pass",
        [path,&charmapper](TreeOperationHandler &handler){
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
}


namespace {
struct SceneWrapper {
  Scene &scene;

  struct BodyWrapper {
    Scene::Body &body;

    bool
      visitOperations(
        const TreePath &,
        int /*depth*/,
        const OperationVisitor &/*visitor*/
      )
    {
      return false;
    }
  };

  bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    )
  {
    int path_length = path.size();

    if (depth==path_length) {
      Scene &scene = this->scene;
      visitor(
        "Add Body",
        [path,&scene](TreeOperationHandler &handler){
          scene.addBody();
          handler.addItem(path,bodyItem());
        }
      );

      return true;
    }

    return
      BodyWrapper{
        scene.bodies[path[depth]]
      }.visitOperations(path,depth+1,visitor);
  }
};
}


bool
  WorldWrapper::visitOperations(
    const TreePath &path,int depth,const OperationVisitor &visitor
  )
{
  int path_length = path.size();

  if (depth==path_length) {
    visitor(
      "Add Charmapper",
      [path,this](TreeOperationHandler &handler){
        world.addCharmapper();
        handler.addItem(path,charmapperItem());
      }
    );
    visitor(
      "Add Scene",
      [path,this](TreeOperationHandler &handler){
        world.addScene();
        handler.addItem(path,sceneItem());
      }
    );

    return true;
  }

  int child_index = path[depth];

  struct ChildOperationsVisitor : World::MemberVisitor {
    const TreePath &path;
    const int depth;
    const OperationVisitor &visitor;
    bool &result;

    ChildOperationsVisitor(
      const TreePath &path_arg,
      int depth_arg,
      const OperationVisitor &visitor_arg,
      bool &result_arg
    )
    : path(path_arg),
      depth(depth_arg),
      visitor(visitor_arg),
      result(result_arg)
    {
    }

    virtual void visitCharmapper(Charmapper &charmapper)
    {
      result =
        CharmapperWrapper{charmapper}.visitOperations(path,depth,visitor);
    }

    virtual void visitScene(Scene &scene)
    {
      SceneWrapper{scene}.visitOperations(path,depth,visitor);
    }
  };

  bool result = false;

  ChildOperationsVisitor operations_visitor(path,depth+1,visitor,result);

  world.visitMember(child_index,operations_visitor);

  return result;
}
