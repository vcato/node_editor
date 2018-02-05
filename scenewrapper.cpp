#include "scenewrapper.hpp"

using OperationVisitor = TreeItem::OperationVisitor;


static TreeItem bodyItem()
{
  return TreeItem();
}


namespace {
struct BodyWrapper : SimpleWrapper {
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
    visitChildWrapper(
      const TreePath &/*path*/,
      int /*depth*/,
      const WrapperVisitor &/*visitor*/
    ) const
  {
    assert(false);
  }

  virtual void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Body");
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
  SceneWrapper::visitChildWrapper(
    const TreePath &path,
    int depth,
    const WrapperVisitor &visitor
  ) const
{
  BodyWrapper{
    scene.bodies[path[depth]]
  }.visitWrapper(path,depth+1,visitor);
}
