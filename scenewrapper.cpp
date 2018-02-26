#include "scenewrapper.hpp"

#include <functional>
#include "float.hpp"


using std::cerr;
using std::vector;
using std::string;
using NotifyFunction = SceneWrapper::NotifyFunction;
#if USE_POINT2D_MAP
using Point2DMap = Scene::Point2DMap;
using Frame = Scene::Frame;
using FloatMap = Scene::FloatMap;
#endif


namespace {
  struct StubOperationHandler : Wrapper::OperationHandler {
    virtual void addItem(const TreePath &)
    {
      assert(false);
    }

    virtual void replaceTreeItems(const TreePath &)
    {
      assert(false);
    }

    virtual void changeEnumerationValues(const TreePath &) const
    {
    }
  };
}


namespace {
struct FloatWrapper : NoOperationWrapper<LeafWrapper<NumericWrapper>> {
  const char *label_member;
#if USE_POINT2D_MAP
  Frame &frame;
  FloatMap &map;
#else
  float &value;
#endif
  const NotifyFunction &notify;

  FloatWrapper(
    const char *label,
#if USE_POINT2D_MAP
    Frame &frame_arg,
    Scene::FloatMap &map_arg,
#else
    float &value_arg,
#endif
    const NotifyFunction &notify_arg
  )
  : label_member(label),
#if USE_POINT2D_MAP
    frame(frame_arg),
    map(map_arg),
#else
    value(value_arg),
#endif
    notify(notify_arg)
  {
  }

  std::string label() const override
  {
    return label_member;
  }

#if USE_POINT2D_MAP
  void setValue(int arg) const override
  {
    frame[map.var_index] = arg;
    StubOperationHandler operation_handler;
    notify(operation_handler);
  }
#else
  void setValue(int arg) const override
  {
    value = arg;
    StubOperationHandler operation_handler;
    notify(operation_handler);
  }
#endif
};
}


namespace {
struct Point2DWrapper : NoOperationWrapper<VoidWrapper> {
  const char *label_member;
#if USE_POINT2D_MAP
  Point2DMap &point;
#else
  Point2D &point;
#endif
  const NotifyFunction &notify;

  Point2DWrapper(
    const char *label_arg,
#if USE_POINT2D_MAP
    Point2DMap &point_arg,
#else
    Point2D &point_arg,
#endif
    const NotifyFunction &notify_arg
  )
  : label_member(label_arg),
    point(point_arg),
    notify(notify_arg)
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

  std::string label() const override
  {
    return label_member;
  }
};
}


namespace {
struct NameWrapper : NoOperationWrapper<LeafWrapper<StringWrapper>> {
  const char *label_member;
  const std::string &name;

  NameWrapper(const char *label,const std::string &name_arg)
  : label_member(label),
    name(name_arg)
  {
  }

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

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeOperationHandler &handler
    )
  {
    switch (operation_index) {
      case 0:
        {
          int index = body.nChildren();
          scene.addChildBodyTo(body);
          notify(handler);
          handler.addItem(join(path,index+nBodyAttributes()));
        }
        return;
    }
    assert(false);
  }

  vector<string> operationNames() const
  {
    return {"Add Body"};
  }

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      OperationHandler &handler
    ) const
  {
    switch (operation_index) {
      case 0:
        {
          int index = body.nChildren();
          scene.addChildBodyTo(body);
          notify(handler);
          handler.addItem(join(path,index+nBodyAttributes()));
        }
        return;
    }

    assert(false);
  }

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


std::vector<std::string> SceneWrapper::operationNames() const
{
  return {"Add Body"};
}


void
  SceneWrapper::executeOperation(
    int operation_index,
    const TreePath &path,
    OperationHandler &handler
  ) const
{
  switch (operation_index) {
    case 0:
      {
        int index = scene.nBodies();
        scene.addBody();
        handler.addItem(join(path,index));
        notify(handler);
      }
      return;
  }

  assert(false);
}


void
  SceneWrapper::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  visitor(BodyWrapper{scene,scene.bodies()[child_index],notify});
}
