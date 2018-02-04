#include "charmapperwrapper.hpp"

#include "worldpolicies.hpp"

using OperationVisitor = TreeItem::OperationVisitor;


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


namespace {
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
}

bool
  CharmapperWrapper::visitOperations(
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
