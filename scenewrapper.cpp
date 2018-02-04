#include "scenewrapper.hpp"

#include "worldpolicies.hpp"


using OperationVisitor = TreeItem::OperationVisitor;


static TreeItem bodyItem()
{
  return TreeItem(world_policies::BodyPolicy{});
}


namespace {
struct BodyWrapper : Wrapper {
  Scene::Body &body;

  BodyWrapper(Scene::Body &body_arg)
  : body(body_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &,
      const TreeItem::OperationVisitor &
    ) const
  {
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void
    visitWrapper(
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

    assert(false);
  }
};
}


void
  SceneWrapper::visitOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
  Scene &scene = this->scene;
  visitor(
    "Add Body",
    [path,&scene](TreeOperationHandler &handler){
      scene.addBody();
      handler.addItem(path,bodyItem());
    }
  );
}


void
  SceneWrapper::visitWrapper(
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

  BodyWrapper{
    scene.bodies[path[depth]]
  }.visitWrapper(path,depth+1,visitor);
}
