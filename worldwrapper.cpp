#include "worldwrapper.hpp"

#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"


using OperationVisitor = TreeItem::OperationVisitor;


static TreeItem charmapperItem()
{
  return TreeItem();
}


static TreeItem sceneItem()
{
  return TreeItem();
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


namespace {
struct ChildWrapperVisitor2 : World::MemberVisitor {
  const std::function<void(const Wrapper&)> &visitor;

  ChildWrapperVisitor2(
    const std::function<void(const Wrapper&)> &visitor_arg
  )
  : visitor(visitor_arg)
  {
  }

  virtual void visitCharmapper(Charmapper &charmapper)
  {
    visitor(CharmapperWrapper{charmapper});
  }

  virtual void visitScene(Scene &scene)
  {
    visitor(SceneWrapper{scene});
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


void
  WorldWrapper::visitChildWrapper(
    const TreePath &path,
    int depth,
    const std::function<void(const Wrapper &)> &visitor
  ) const
{
  int child_index = path[depth];

  ChildWrapperVisitor wrapper_visitor(path,depth+1,visitor);

  world.visitMember(child_index,wrapper_visitor);
}


#if 1
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

  visitChildWrapper(path,depth,visitor);
}
#endif
