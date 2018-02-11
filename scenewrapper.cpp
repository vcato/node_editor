#include "scenewrapper.hpp"


using std::cerr;
using NotifyFunction = std::function<void()>;


namespace {
struct FloatWrapper : NumericWrapper {
  const char *label_member;
  Scene &scene;
  float &value;
  const NotifyFunction &notify;

  FloatWrapper(
    const char *label,
    float &value_arg,
    Scene &scene_arg,
    const NotifyFunction &notify_arg
  )
  : label_member(label),
    scene(scene_arg),
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
  Scene &scene;
  const NotifyFunction &notify;

  Point2DWrapper(
    const char *label_arg,
    Point2D &point_arg,
    Scene &scene_arg,
    const NotifyFunction &notify_arg
  )
  : label_member(label_arg),
    point(point_arg),
    scene(scene_arg),
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
        visitor(FloatWrapper("x",point.x,scene,notify));
        return;
      case 1:
        visitor(FloatWrapper("y",point.y,scene,notify));
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
  Scene &scene;
  const NotifyFunction &notify;

  BodyWrapper(
    Scene::Body &body_arg,
    Scene &scene_arg,
    const NotifyFunction &notify_arg
  )
  : body(body_arg),
    scene(scene_arg),
    notify(notify_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const
  {
    const NotifyFunction &notify = this->notify;
    Scene::Body &body = this->body;
    visitor(
      "Add Body",
      [notify,&body,path](TreeOperationHandler &handler){
	int index = body.nChildren();
	body.addChild();
	notify();
	handler.addItem(join(path,index+1));
      }
    );
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    if (child_index==0) {
      visitor(Point2DWrapper("position",body.position,scene,notify));
      return;
    }

    visitor(BodyWrapper(body.children[child_index-1],scene,notify));
  }

  virtual std::string label() const
  {
    return "Body";
  }

  virtual int nChildren() const
  {
    return 1 + body.nChildren();
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
  Scene &scene = this->scene;
  const NotifyFunction &notify = this->notify;
  visitor(
    "Add Body",
    [path,&scene,notify](TreeOperationHandler &handler){
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
  visitor(BodyWrapper{scene.bodies()[child_index],scene,notify});
}
