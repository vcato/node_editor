#include "scenewrapper.hpp"


namespace {
struct Point2DWrapper : Wrapper {
  const char *label_member;

  Point2DWrapper(const char *label_arg)
  : label_member(label_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &,
      const OperationVisitor &
    ) const
  {
  }

  virtual void
    withChildWrapper(
      int /*child_index*/,
      const WrapperVisitor &/*visitor*/
    ) const
  {
    assert(false);
  }

  virtual int nChildren() const
  {
    return 0; // this is wrong
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  virtual void
    comboBoxItemIndexChanged(
      const TreePath &/*path*/,
      int /*index*/,
      OperationHandler &/*operation_handler*/
    ) const
  {
    assert(false);
  }

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.voidItem();
  }

  std::string label() const override
  {
    return label_member;
  }
};
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
      const OperationVisitor &
    ) const
  {
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    if (child_index==0) {
      visitor(Point2DWrapper("position"));
      return;
    }

    assert(false);
  }

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.voidItem();
  }

  virtual std::string label() const
  {
    return "Body";
  }

  virtual int nChildren() const
  {
    return 1;
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
      int index = scene.nBodies();
      scene.addBody();
      handler.addItem(join(path,index));
    }
  );
}


void
  SceneWrapper::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  visitor(BodyWrapper{scene.bodies()[child_index]});
}
