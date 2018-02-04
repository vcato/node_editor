#include "charmapperwrapper.hpp"

#include <iostream>
#include "worldpolicies.hpp"

using std::cerr;
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
struct MotionPassWrapper : Wrapper {
  Charmapper::MotionPass &motion_pass;

  MotionPassWrapper(Charmapper::MotionPass &motion_pass_arg)
  : motion_pass(motion_pass_arg)
  {
  }

  virtual void
    visitOperations(const TreePath &path,const OperationVisitor &visitor) const
  {
    Charmapper::MotionPass &motion_pass = this->motion_pass;
    visitor(
      "Add Pos Expr",
      [path,&motion_pass](TreeOperationHandler &handler){
      motion_pass.addPosExpr();
      handler.addItem(path,posExprItem());
      }
    );
  }

  void
    visitWrapper(const TreePath &path,int depth,const WrapperVisitor &visitor)
  {
    int path_length = path.size();

    if (depth==path_length) {
      visitor(*this);

      return;
    }

    // This isn't implemented yet, but it doesn't hurt anything so far,
    // since the only the we use the wrapper for is visiting operations.
    // assert(false);
  }
};
}


void
  CharmapperWrapper::visitOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
  Charmapper &charmapper = this->charmapper;
  visitor(
    "Add Motion Pass",
    [path,&charmapper](TreeOperationHandler &handler){
      charmapper.addMotionPass();
      handler.addItem(path,motionPassItem());
    }
  );
}


void
  CharmapperWrapper::visitWrapper(
    const TreePath &path,
    int depth,
    const WrapperVisitor &visitor
  )
{
  int path_length = path.size();

  if (depth==path_length) {
    visitor(*this);

    return;
  }

  int child_index = path[depth];
  assert(charmapper.passes[child_index]);

  MotionPassWrapper child_wrapper{*charmapper.passes[child_index]};
  return child_wrapper.visitWrapper(path,depth+1,visitor);
}
