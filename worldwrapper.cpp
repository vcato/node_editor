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
struct ChildWrapperVisitor : World::MemberVisitor {
  const TreePath &path;
  const int depth;
  const std::function<void(const Wrapper&)> &visitor;

  ChildWrapperVisitor(
    const TreePath &path_arg,
    int depth_arg,
    const std::function<void(const Wrapper&)> &visitor_arg
  )
  : path(path_arg),
    depth(depth_arg),
    visitor(visitor_arg)
  {
  }

  virtual void visitCharmapper(Charmapper &charmapper)
  {
    CharmapperWrapper{charmapper}.visitWrapper(path,depth,visitor);
  }

  virtual void visitScene(Scene &scene)
  {
    SceneWrapper{scene}.visitWrapper(path,depth,visitor);
    // visitor(SceneWrapper{scene});
  }
};
}


void
  WorldWrapper::visitOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
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
}


bool
  WorldWrapper::visitOperations(
    const TreePath &path,int depth,const OperationVisitor &/*visitor*/
  )
{
  int path_length = path.size();

  if (depth==path_length) {
    assert(false);
    return false;
  }

  return false;
}


void
  WorldWrapper::visitWrapper(
    const TreePath &path,
    int depth,
    const std::function<void(const Wrapper &)> &visitor
  ) const
{
  int path_length = path.size();

  if (depth==path_length) {
    visitor(*this);
    return;
  }

  ChildWrapperVisitor wrapper_visitor(path,depth+1,visitor);

  int child_index = path[depth];

  world.visitMember(child_index,wrapper_visitor);
}
