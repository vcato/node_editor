#include "scenewrapper.hpp"


namespace {
struct FloatWrapper : NumericWrapper {
  const char *label_member;

  FloatWrapper(const char *label,float &)
  : label_member(label)
  {
  }

  void
    visitOperations(
      const TreePath &,
      const OperationVisitor &
    ) const override
  {
  }

  void
    withChildWrapper(
      int /*child_index*/,
      const WrapperVisitor &
    ) const override
  {
  }

  int nChildren() const override { return 0; }

  Diagram *diagramPtr() const override { return nullptr; }

  std::string label() const override
  {
    return label_member;
  }
};
}


namespace {
struct Point2DWrapper : VoidWrapper {
  const char *label_member;
  Point2D &point;

  Point2DWrapper(const char *label_arg,Point2D &point_arg)
  : label_member(label_arg),
    point(point_arg)
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
      int child_index,
      const WrapperVisitor &visitor
    ) const
  {
    switch (child_index) {
      case 0:
        visitor(FloatWrapper("x",point.x));
        return;
      case 1:
        visitor(FloatWrapper("y",point.y));
        return;
    }

    assert(false);
  }

  int nChildren() const override { return 2; }

  virtual Diagram *diagramPtr() const { return nullptr; }

  std::string label() const override
  {
    return label_member;
  }
};
}


namespace {
struct BodyWrapper : VoidWrapper {
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
      visitor(Point2DWrapper("position",body.position));
      return;
    }

    assert(false);
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
