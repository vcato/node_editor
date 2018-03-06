#include "scenewrapper.hpp"

#include <functional>
#include "float.hpp"


using std::cerr;
using std::vector;
using std::string;
using NotifyFunction = SceneWrapper::NotifyFunction;
using Point2DMap = Scene::Point2DMap;
using Frame = Scene::Frame;
using FloatMap = Scene::FloatMap;
using Label = SceneWrapper::Label;


namespace {
struct WrapperData {
  Scene &scene;
  const NotifyFunction &notify;
  Frame &frame;
};
}


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
struct FloatMapWrapper : NoOperationWrapper<LeafWrapper<NumericWrapper>> {
  const char *label_member;
  Scene::FloatMap &map;
  WrapperData wrapper_data;

  FloatMapWrapper(
    const char *label,
    Scene::FloatMap &map_arg,
    WrapperData wrapper_data_arg
  )
  : label_member(label),
    map(map_arg),
    wrapper_data(wrapper_data_arg)
  {
  }

  Label label() const override
  {
    return label_member;
  }

  void setValue(int arg) const override
  {
    map.set(wrapper_data.frame,arg);
    StubOperationHandler operation_handler;
    wrapper_data.notify(operation_handler);
  }
};
}


namespace {
struct FloatWrapper : NoOperationWrapper<LeafWrapper<NumericWrapper>> {
  const char *label_member;
  float &value;
  WrapperData wrapper_data;

  FloatWrapper(
    const char *label,
    float &value_arg,
    WrapperData wrapper_data_arg
  )
  : label_member(label),
    value(value_arg),
    wrapper_data(wrapper_data_arg)
  {
  }

  Label label() const override
  {
    return label_member;
  }

  void setValue(int arg) const override
  {
    value = arg;
    StubOperationHandler operation_handler;
    wrapper_data.notify(operation_handler);
  }
};
}


namespace {
struct Point2DWrapper : NoOperationWrapper<VoidWrapper> {
  const char *label_member;
  Point2DMap &point;
  WrapperData wrapper_data;

  Point2DWrapper(
    const char *label_arg,
    Point2DMap &point_arg,
    WrapperData wrapper_data_arg
  )
  : label_member(label_arg),
    point(point_arg),
    wrapper_data(wrapper_data_arg)
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
        visitor(FloatMapWrapper("x",point.x,wrapper_data));
        return;
      case 1:
        visitor(FloatMapWrapper("y",point.y,wrapper_data));
        return;
    }

    assert(false);
  }

  int nChildren() const override { return 2; }

  Label label() const override
  {
    return label_member;
  }
};
}


namespace {
struct NameWrapper : NoOperationWrapper<LeafWrapper<StringWrapper>> {
  const char *label_member;
  std::string &name;
  WrapperData wrapper_data;

  NameWrapper(
    const char *label,
    string &name_arg,
    const WrapperData &wrapper_data_arg
  )
  : label_member(label),
    name(name_arg),
    wrapper_data(wrapper_data_arg)
  {
  }

  Label label() const override { return label_member; }

  virtual std::string value() const
  {
    return name;
  }

  void setValue(const string &arg) const
  {
    name = arg;
    StubOperationHandler operation_handler;
    wrapper_data.notify(operation_handler);
  }
};
}


namespace {
struct BodyWrapper : VoidWrapper {
  Scene &scene;
  Scene::Body &body;
  WrapperData wrapper_data;
  static int nBodyAttributes() { return 2; }

  BodyWrapper(
    Scene &scene_arg,
    Scene::Body &body_arg,
    WrapperData wrapper_data_arg
  )
  : scene(scene_arg),
    body(body_arg),
    wrapper_data(wrapper_data_arg)
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
          wrapper_data.notify(handler);
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
          wrapper_data.notify(handler);
          handler.addItem(join(path,index+nBodyAttributes()));
        }
        return;
    }

    assert(false);
  }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    if (child_index==0) {
      visitor(NameWrapper{"name",body.name,wrapper_data});
      return;
    }

    if (child_index==1) {
      visitor(Point2DWrapper("position",body.position,wrapper_data));
      return;
    }

    Scene::Body &child = body.children[child_index-nBodyAttributes()];

    visitor(BodyWrapper(scene,child,wrapper_data));
  }

  virtual Label label() const
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
  NotifyFunction notify_arg,
  const Label &label_arg
)
: scene(scene_arg),
  notify(notify_arg),
  label_member(label_arg)
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
  WrapperData wrapper_data = {scene,notify,scene.backgroundFrame()};
  visitor(
    BodyWrapper{scene,scene.bodies()[child_index],wrapper_data}
  );
}


Label SceneWrapper::label() const
{
  return label_member;
}
