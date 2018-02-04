#include "worldwrapper.hpp"

#include "worldpolicies.hpp"
#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"


using OperationVisitor = TreeItem::OperationVisitor;


static TreeItem charmapperItem()
{
  return TreeItem(world_policies::CharmapperPolicy{});
}


static TreeItem sceneItem()
{
  return TreeItem(world_policies::ScenePolicy{});
}


namespace {
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
    result =
      SceneWrapper{scene}.visitOperations(path,depth,visitor);
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
  bool result = false;
  ChildOperationsVisitor operations_visitor(path,depth+1,visitor,result);

  world.visitMember(child_index,operations_visitor);

  return result;
}
