#include "scenewrapper.hpp"

#include <functional>
#include "float.hpp"


using std::cerr;
using std::vector;
using std::string;
using Callbacks = SceneWrapper::SceneObserver;
using Point2DMap = Scene::Point2DMap;
using Frame = Scene::Frame;
using FloatMap = Scene::FloatMap;
using Label = SceneWrapper::Label;


namespace {
struct WrapperData {
  Scene &scene;
  const Callbacks &callbacks;
  Frame &frame;
};
}


namespace {
  struct StubTreeObserver : Wrapper::TreeObserver {
    virtual void itemAdded(const TreePath &)
    {
      assert(false);
    }

    virtual void itemReplaced(const TreePath &)
    {
      assert(false);
    }

    virtual void enumarationValuesChanged(const TreePath &) const
    {
    }

    virtual void itemRemoved(const TreePath &)
    {
      assert(false);
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
    StubTreeObserver tree_observer;
    wrapper_data.callbacks.changed_func(tree_observer);
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
    StubTreeObserver tree_observer;
    wrapper_data.callbacks.changed_func(tree_observer);
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
    StubTreeObserver tree_observer;
    wrapper_data.callbacks.changed_func(tree_observer);
  }
};
}


namespace {
struct BodyWrapper : VoidWrapper {
  Scene &scene;
  Scene::Body &parent_body;
  int body_index;
  Scene::Body &body;
  WrapperData wrapper_data;
  static int nBodyAttributes() { return 2; }

  BodyWrapper(
    Scene &scene_arg,
    Scene::Body &parent_body_arg,
    int body_index_arg,
    WrapperData wrapper_data_arg
  )
  : scene(scene_arg),
    parent_body(parent_body_arg),
    body_index(body_index_arg),
    body(parent_body_arg.children[body_index_arg]),
    wrapper_data(wrapper_data_arg)
  {
  }

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &tree_observer
    )
  {
    switch (operation_index) {
      case 0:
        {
          int index = body.nChildren();
          scene.addChildBodyTo(body);
          wrapper_data.callbacks.changed_func(tree_observer);
          tree_observer.itemAdded(join(path,index+nBodyAttributes()));
        }
        return;
    }
    assert(false);
  }

  vector<string> operationNames() const
  {
    return {"Add Body","Remove"};
  }

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &tree_observer
    ) const
  {
    switch (operation_index) {
      case 0:
        {
          int index = body.nChildren();
          Scene::Body &new_body = scene.addChildBodyTo(body);

          if (wrapper_data.callbacks.body_added_func) {
            wrapper_data.callbacks.body_added_func(new_body,tree_observer);
          }

          tree_observer.itemAdded(join(path,index+nBodyAttributes()));
        }
        return;
      case 1:
        {
          if (wrapper_data.callbacks.removing_body_func) {
            wrapper_data.callbacks.removing_body_func(body);
          }

          scene.removeChildBodyFrom(parent_body,body_index);
          tree_observer.itemRemoved(path);

          if (wrapper_data.callbacks.removed_body_func) {
            wrapper_data.callbacks.removed_body_func(tree_observer);
          }
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

    int body_index = child_index-nBodyAttributes();
    visitor(BodyWrapper(scene,body,body_index,wrapper_data));
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
  const Callbacks &notify_arg,
  const Label &label_arg
)
: scene(scene_arg),
  callbacks(notify_arg),
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
    TreeObserver &tree_observer
  ) const
{
  switch (operation_index) {
    case 0:
      {
        int index = scene.nBodies();
        Scene::Body &new_body = scene.addBody();

        tree_observer.itemAdded(join(path,index));

        if (callbacks.body_added_func) {
          callbacks.body_added_func(new_body,tree_observer);
        }
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
  WrapperData wrapper_data = {scene,callbacks,scene.backgroundFrame()};
  visitor(
    BodyWrapper{scene,scene.rootBody(),child_index,wrapper_data}
  );
}


Label SceneWrapper::label() const
{
  return label_member;
}
