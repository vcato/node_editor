#include "scenewrapper.hpp"

#include <functional>
#include <sstream>
#include "float.hpp"


using std::cerr;
using std::vector;
using std::string;
using std::ostringstream;
using std::ostream;
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
    return Label(label_member)+" variable";
  }

  void setValue(int arg) const override
  {
    if (arg<0 || arg>=wrapper_data.frame.nVariables()) {
      return;
    }

    map.var_index = arg;
    StubTreeObserver tree_observer;
    wrapper_data.callbacks.changed_func(tree_observer);
  }

  virtual Value value() const
  {
    return map.var_index;
  }
};
}


namespace {
struct FloatWrapper : NoOperationWrapper<LeafWrapper<NumericWrapper>> {
  const char *label_member;
  float &value_ref;
  WrapperData wrapper_data;

  FloatWrapper(
    const char *label,
    float &value_arg,
    WrapperData &wrapper_data_arg
  )
  : label_member(label),
    value_ref(value_arg),
    wrapper_data(wrapper_data_arg)
  {
  }

  Label label() const override
  {
    return label_member;
  }

  void setValue(int arg) const override
  {
    value_ref = arg;
    StubTreeObserver tree_observer;
    wrapper_data.callbacks.changed_func(tree_observer);
  }

  virtual Value value() const { return value_ref; }
};
}


namespace {
struct Point2DMapWrapper : NoOperationWrapper<VoidWrapper> {
  const char *label_member;
  Point2DMap &point;
  WrapperData wrapper_data;

  Point2DMapWrapper(
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

  void setState(const WrapperState &state)
  {
    for (const WrapperState &child_state : state.children) {
      if (child_state.tag=="x") {
        assert(false);
      }
      else {
        assert(false);
      }
    }
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
  int depth;
  Scene::Body &body;
  WrapperData wrapper_data;
  static int nBodyAttributes() { return 2; }

  BodyWrapper(
    Scene &scene_arg,
    Scene::Body &parent_body_arg,
    int body_index_arg,
    WrapperData wrapper_data_arg,
    int depth_arg
  )
  : scene(scene_arg),
    parent_body(parent_body_arg),
    body_index(body_index_arg),
    depth(depth_arg),
    body(parent_body_arg.children[body_index_arg]),
    wrapper_data(wrapper_data_arg)
  {
  }

  vector<string> operationNames() const override;

  void executeAddBody(const TreePath &, TreeObserver &) const;
  void executeRemove(const TreePath &, TreeObserver &) const;

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &tree_observer
    ) const;

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    if (child_index==0) {
      visitor(NameWrapper{"name",body.name,wrapper_data});
      return;
    }

    if (child_index==1) {
      visitor(Point2DMapWrapper("position map",body.position,wrapper_data));
      return;
    }

    int body_index = child_index-nBodyAttributes();
    visitor(BodyWrapper(scene,body,body_index,wrapper_data,depth + 1));
  }

  virtual Label label() const
  {
    return "Body";
  }

  virtual int nChildren() const
  {
    return nBodyAttributes() + body.nChildren();
  }

  void setState(const WrapperState &state)
  {
    if (body.nChildren()!=0) {
      assert(false);
    }

    for (const WrapperState &child_state : state.children) {
      if (child_state.tag=="name") {
        body.name = child_state.value.asString();
      }
      else if (child_state.tag=="position") {
        Point2DMapWrapper(
          "position",
          body.position,
          wrapper_data
        ).setState(child_state);
      }
      else {
        cerr << "child_state.tag: " << child_state.tag << "\n";
        assert(false);
      }
    }
  }
};
}


vector<string> BodyWrapper::operationNames() const
{
  return {"Add Body","Remove"};
}


void
  BodyWrapper::executeAddBody(
    const TreePath &path,
    TreeObserver &tree_observer
  ) const
{
  int index = body.nChildren();
  Frame &frame = scene.backgroundFrame();
  int old_n_vars = frame.nVariables();
  Scene::Body &new_body = scene.addChildBodyTo(body);
  int new_n_vars = frame.nVariables();

  if (new_n_vars!=old_n_vars) {
    TreePath scene_path = path;

    scene_path.erase(scene_path.end()-depth,scene_path.end());
    TreePath background_frame_path = join(scene_path,0);

    for (int i=old_n_vars; i!=new_n_vars; ++i) {
      TreePath var_path = join(background_frame_path,i);
      tree_observer.itemAdded(var_path);
    }
  }

  if (wrapper_data.callbacks.body_added_func) {
    wrapper_data.callbacks.body_added_func(new_body,tree_observer);
  }

  tree_observer.itemAdded(join(path,index+nBodyAttributes()));
}


void
  BodyWrapper::executeRemove(
    const TreePath &path,
    TreeObserver &tree_observer
  ) const
{
  Frame &frame = scene.backgroundFrame();

  if (wrapper_data.callbacks.removing_body_func) {
    wrapper_data.callbacks.removing_body_func(body);
  }

  int old_n_vars = frame.nVariables();

  scene.removeChildBodyFrom(parent_body,body_index);

  int new_n_vars = frame.nVariables();

  // Extra variables should probably be removed, but they aren't currently.
  assert(old_n_vars==new_n_vars);

  tree_observer.itemRemoved(path);

  if (wrapper_data.callbacks.removed_body_func) {
    wrapper_data.callbacks.removed_body_func(tree_observer);
  }
}


void
  BodyWrapper::executeOperation(
    int operation_index,
    const TreePath &path,
    TreeObserver &tree_observer
  ) const
{
  switch (operation_index) {
    case 0:
      executeAddBody(path,tree_observer);
      return;
    case 1:
      executeRemove(path,tree_observer);
      return;
  }

  assert(false);
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
  SceneWrapper::executeAddBody(
    const TreePath &scene_path,
    TreeObserver &tree_observer
  ) const
{
  int index = scene.nBodies();
  Frame &frame = scene.backgroundFrame();
  int old_n_vars = frame.nVariables();
  Scene::Body &new_body = scene.addBody();
  int new_n_vars = frame.nVariables();

  if (new_n_vars!=old_n_vars) {
    assert(new_n_vars>old_n_vars);
    TreePath background_frame_path = join(scene_path,0);

    for (int i=old_n_vars; i!=new_n_vars; ++i) {
      TreePath var_path = join(background_frame_path,i);
      tree_observer.itemAdded(var_path);
    }
  }

  tree_observer.itemAdded(join(scene_path,index+firstBodyIndex()));

  if (callbacks.body_added_func) {
    callbacks.body_added_func(new_body,tree_observer);
  }
}


void
  SceneWrapper::executeOperation(
    int operation_index,
    const TreePath &scene_path,
    TreeObserver &tree_observer
  ) const
{
  switch (operation_index) {
    case 0:
      executeAddBody(scene_path,tree_observer);
      return;
  }

  assert(false);
}


struct SceneWrapper::FrameWrapper : NoOperationWrapper<VoidWrapper> {
  string label_member;
  Frame &frame;
  WrapperData &wrapper_data;

  FrameWrapper(
    const char *label,
    Frame &frame_arg,
    WrapperData &wrapper_data_arg
  )
  : label_member(label),
    frame(frame_arg),
    wrapper_data(wrapper_data_arg)
  {
  }

  int nChildren() const override
  {
    return frame.nVariables();
  }

  virtual Label label() const { return label_member; }

  void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const override
  {
    ostringstream stream;
    stream << child_index;
    visitor(
      FloatWrapper(
        stream.str().c_str(),
        frame.var_values[child_index],
        wrapper_data
      )
    );
  }

  void setState(const WrapperState &new_state) const
  {
    if (frame.nVariables()==0 && new_state.children.size()==0) {
      return;
    }

    assert(false);
  }
};


void
  SceneWrapper::withBackgroundFrameWrapper(
    const std::function<void(const FrameWrapper &)> &visitor
  ) const
{
  WrapperData wrapper_data = {scene,callbacks,scene.backgroundFrame()};

  visitor(
    FrameWrapper(
      "background_frame",
      scene.backgroundFrame(),
      wrapper_data
    )
  );
}


void
  SceneWrapper::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  WrapperData wrapper_data = {scene,callbacks,scene.backgroundFrame()};

  if (child_index==0) {
    withBackgroundFrameWrapper(visitor);
    return;
  }

  visitor(
    BodyWrapper{
      scene,
      scene.rootBody(),
      child_index - firstBodyIndex(),
      wrapper_data,
      /*depth*/1
    }
  );
}


int SceneWrapper::nChildren() const
{
  return 1 + scene.nBodies();
}


Label SceneWrapper::label() const
{
  return label_member;
}


void SceneWrapper::setState(const WrapperState &state)
{
  if (state.children.empty()) {
    return;
  }

  if (scene.nBodies()!=0) {
    assert(false);
  }

  WrapperData wrapper_data = {scene,callbacks,scene.backgroundFrame()};
  int child_index = 0;

  for (const WrapperState &child_state : state.children) {
    if (child_state.tag=="background_frame") {
      withBackgroundFrameWrapper(
        [&](const FrameWrapper &frame_wrapper){
          frame_wrapper.setState(child_state);
        }
      );
    }
    else if (child_state.tag=="Body") {
      scene.addBody();
      Scene::Body &parent_body = scene.rootBody();
      BodyWrapper(
        scene,parent_body,child_index,wrapper_data,/*depth*/1
      ).setState(child_state);
    }
    else {
      assert(false);
    }

    ++child_index;
  }
}
