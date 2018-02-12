#include "scenewrapper.hpp"


using std::cerr;
using NotifyFunction = std::function<void()>;


namespace {
struct FloatWrapper : NumericWrapper {
  const char *label_member;
  float &value;
  const NotifyFunction &notify;

  FloatWrapper(
    const char *label,
    float &value_arg,
    const NotifyFunction &notify_arg
  )
  : label_member(label),
    value(value_arg),
    notify(notify_arg)
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

  void setValue(int arg) const override
  {
    value = arg;
    notify();
  }
};
}


namespace {
struct Point2DWrapper : VoidWrapper {
  const char *label_member;
  Point2D &point;
  const NotifyFunction &notify;

  Point2DWrapper(
    const char *label_arg,
    Point2D &point_arg,
    const NotifyFunction &notify_arg
  )
  : label_member(label_arg),
    point(point_arg),
    notify(notify_arg)
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
        visitor(FloatWrapper("x",point.x,notify));
        return;
      case 1:
        visitor(FloatWrapper("y",point.y,notify));
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
struct NameWrapper : StringWrapper {
  const char *label_member;
  const std::string &name;

  NameWrapper(const char *label,const std::string &name_arg)
  : label_member(label),
    name(name_arg)
  {
  }

  void
    visitOperations(
      const TreePath &,
      const OperationVisitor &
    ) const override
  {
  }

  int nChildren() const override { return 0; }

  void
    withChildWrapper(
      int /*child_index*/,
      const WrapperVisitor &
    ) const override
  {
    assert(false);
  }

  Diagram *diagramPtr() const override { return nullptr; }

  std::string label() const override { return label_member; }

  virtual std::string value() const
  {
    return name;
  }
};
}


namespace {
struct BodyWrapper : VoidWrapper {
  Scene &scene;
  Scene::Body &body;
  const NotifyFunction &notify;
  static int nBodyAttributes() { return 2; }

  BodyWrapper(
    Scene &scene_arg,
    Scene::Body &body_arg,
    const NotifyFunction &notify_arg
  )
  : scene(scene_arg),
    body(body_arg),
    notify(notify_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const
  {
    visitor(
      "Add Body",
      [notify=notify,&body=body,&scene=scene,path](
        TreeOperationHandler &handler
      ){
	int index = body.nChildren();
	scene.addChildBodyTo(body);
	notify();
	handler.addItem(join(path,index+nBodyAttributes()));
      }
    );
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    if (child_index==0) {
      visitor(NameWrapper{"name",body.name});
      return;
    }

    if (child_index==1) {
      visitor(Point2DWrapper("position",body.position,notify));
      return;
    }

    Scene::Body &child = body.children[child_index-nBodyAttributes()];

    visitor(BodyWrapper(scene,child,notify));
  }

  virtual std::string label() const
  {
    return "Body";
  }

  virtual int nChildren() const
  {
    return nBodyAttributes() + body.nChildren();
  }
};
}


SceneWrapper::SceneWrapper(
  Scene &scene_arg,
  NotifyFunction notify_arg
)
: scene(scene_arg),
  notify(notify_arg)
{
}


void
  SceneWrapper::visitOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
  visitor(
    "Add Body",
    [path,&scene=scene,notify=notify](TreeOperationHandler &handler){
      int index = scene.nBodies();
      scene.addBody();
      notify();
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
  visitor(BodyWrapper{scene,scene.bodies()[child_index],notify});
}
