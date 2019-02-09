#include "scenewrapper.hpp"

#include <sstream>
#include "streamvector.hpp"
#include "wrapperutil.hpp"
#include "basicvariant.hpp"
#include "wrapperstate.hpp"
#include "makestr.hpp"

using std::istringstream;
using std::ostringstream;
using std::ostream;
using std::string;
using std::cerr;
using std::vector;


static WrapperState stateFromText(const char *text)
{
  istringstream stream(text);
  ScanStateResult result = scanStateFrom(stream);
  assert(!result.isError());
  return result.state();
}


static void setWrapperStateFromText(const Wrapper &wrapper,const char *text)
{
  wrapper.setState(stateFromText(text));
}


static void printTree(ostream &stream,const Wrapper &wrapper,int indent = 0)
{
  printStateOn(stream,stateOf(wrapper),indent);
}


static SceneWrapper::SceneObserver unusedObserver()
{
  return
    SceneWrapper::SceneObserver(
      [](){ assert(false); }
    );
}


static SceneWrapper::SceneObserver ignoringObserver()
{
  return
    SceneWrapper::SceneObserver(
      /*changed_func*/ [](){}
    );
}


namespace {
struct TreeObserverStub : Wrapper::TreeObserver {
  void itemAdded(const TreePath &) override
  {
  }

  void itemReplaced(const TreePath &) override
  {
    assert(false);
  }

  void itemRemoved(const TreePath &) override
  {
    assert(false);
  }
};
}


static void testHierarchy()
{
  Scene scene;
  Scene::Body &body = scene.addBody();
  scene.addChildBodyTo(body);
  SceneWrapper::SceneObserver observer = unusedObserver();
  SceneWrapper wrapper(scene,&observer,"Scene");
  ostringstream stream;
  printTree(stream,wrapper);
  string output = stream.str();

  auto expected_output =
    "scene {\n"
    "  background_motion {\n"
    "    0 {\n"
    "      0: 0\n"
    "      1: 0\n"
    "      2: 0\n"
    "      3: 0\n"
    "    }\n"
    "  }\n"
    "  current_frame: 0\n"
    "  body {\n"
    "    name: \"Body1\"\n"
    "    position_map {\n"
    "      x_variable: 0\n"
    "      y_variable: 1\n"
    "    }\n"
    "    body {\n"
    "      name: \"Body2\"\n"
    "      position_map {\n"
    "        x_variable: 2\n"
    "        y_variable: 3\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n";

  if (output!=expected_output) {
    cerr << "output:\n";
    cerr << output << "\n";
  }

  assert(output==expected_output);
}


static void
  addBody2(const Wrapper &wrapper,const TreePath &path,ostream &stream)
{
  struct TreeObserver : Wrapper::TreeObserver {
    ostream &stream;

    TreeObserver(ostream &stream_arg)
    : stream(stream_arg)
    {
    }

    void itemAdded(const TreePath &path) override
    {
      stream << "addItem: path=" << path << "\n";
    }

    void itemReplaced(const TreePath &) override
    {
      assert(false);
    }

    void itemRemoved(const TreePath &) override
    {
      assert(false);
    }

    virtual void itemLabelChanged(const TreePath &) override
    {
      assert(false);
    }
  };

  TreeObserver operation_handler(stream);

  executeAddBodyFunction(wrapper,path,operation_handler);
}


static void
  addBodyTo(const Wrapper &wrapper,const TreePath &path,ostream &stream)
{
  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &body_wrapper){
      addBody2(body_wrapper,path,stream);
    }
  );
}


static void testAddingBodies()
{
  Scene scene;
  SceneWrapper::SceneObserver notify([](){});
  SceneWrapper scene_wrapper(scene,&notify,"Scene");
  ostringstream stream;

  int body_index = 2;
  int scene_first_body_child_index = SceneWrapper::firstBodyChildIndex();

  TreePath scene_path = {};
  TreePath background_frame_path = {0,0};
  TreePath first_body_path = {scene_first_body_child_index};
  TreePath first_child_body_path = {scene_first_body_child_index,body_index};
  TreePath first_grandchild_body_path = {
    scene_first_body_child_index,body_index,body_index
  };
  addBodyTo(scene_wrapper,scene_path,stream);
  addBodyTo(scene_wrapper,first_body_path,stream);
  addBodyTo(scene_wrapper,first_child_body_path,stream);

  assert(scene.nBodies()==1);
  assert(scene.bodies()[0].nChildren()==1);
  assert(scene.bodies()[0].allChildren()[0].nChildren()==1);

  string commands = stream.str();
  string expected_commands =
    "addItem: path=" + makeStr(childPath(background_frame_path,0)) + "\n"
    "addItem: path=" + makeStr(childPath(background_frame_path,1)) + "\n"
    "addItem: path=" + makeStr(first_body_path)               + "\n"
    "addItem: path=" + makeStr(childPath(background_frame_path,2)) + "\n"
    "addItem: path=" + makeStr(childPath(background_frame_path,3)) + "\n"
    "addItem: path=" + makeStr(first_child_body_path)         + "\n"
    "addItem: path=" + makeStr(childPath(background_frame_path,4)) + "\n"
    "addItem: path=" + makeStr(childPath(background_frame_path,5)) + "\n"
    "addItem: path=" + makeStr(first_grandchild_body_path)    + "\n";

  if (commands!=expected_commands) {
    cerr << "commands:\n";
    cerr << commands << "\n";
    cerr << "expected_commands:\n";
    cerr << expected_commands << "\n";
  }

  assert(commands==expected_commands);
}


static void testGettingState()
{
  Scene scene;
  SceneWrapper::SceneObserver notify(
    [](){ assert(false); }
  );
  Scene::Body &body = scene.addBody();
  scene.addChildBodyTo(body);
  SceneWrapper wrapper(scene,&notify,"Scene");
  WrapperState state = stateOf(wrapper);
  ostringstream stream;
  printStateOn(stream,state);
  string output = stream.str();

  auto expected_output =
    "scene {\n"
    "  background_motion {\n"
    "    0 {\n"
    "      0: 0\n"
    "      1: 0\n"
    "      2: 0\n"
    "      3: 0\n"
    "    }\n"
    "  }\n"
    "  current_frame: 0\n"
    "  body {\n"
    "    name: \"Body1\"\n"
    "    position_map {\n"
    "      x_variable: 0\n"
    "      y_variable: 1\n"
    "    }\n"
    "    body {\n"
    "      name: \"Body2\"\n"
    "      position_map {\n"
    "        x_variable: 2\n"
    "        y_variable: 3\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n";

  if (output!=expected_output) {
    cerr << "output:\n";
    cerr << output << "\n";
  }

  assert(output==expected_output);
}


static void testSettingStateWithNonEmptyBackgroundFrame()
{
  Scene scene;
  SceneWrapper::SceneObserver observer = ignoringObserver();
  SceneWrapper wrapper(scene,&observer,"Scene");

  const char *text =
    "scene {\n"
    "  background_motion {\n"
    "    0 {\n"
    "      0: 1\n"
    "      1: 3\n"
    "    }\n"
    "  }\n"
    "}\n";

  setWrapperStateFromText(wrapper,text);

  assert(scene.backgroundFrame().nVariables()==2);
  assert(scene.backgroundFrame().var_values[0]==1);
  assert(scene.backgroundFrame().var_values[1]==3);
}


static void testSettingStateWithEmptyBackgroundFrame()
{
  Scene scene;
  SceneWrapper::SceneObserver observer = ignoringObserver();
  SceneWrapper wrapper(scene,&observer,"Scene");
  const char *text =
    "scene {\n"
    "  background_motion {\n"
    "    0 {\n"
    "    }\n"
    "  }\n"
    "  current_frame: 0\n"
    "}\n";
  WrapperState state = stateFromText(text);
  wrapper.setState(state);

  assert(stateOf(wrapper)==state);
}


static void testSettingStateWithTwoBodies()
{
  const char *text =
    "scene {\n"
    "  background_motion {\n"
    "    0 {\n"
    "      0: 48\n"
    "      1: 0\n"
    "      2: 11\n"
    "      3: 57\n"
    "    }\n"
    "  }\n"
    "  current_frame: 0\n"
    "  body {\n"
    "    name: \"Body1\"\n"
    "    position_map {\n"
    "      x_variable: 0\n"
    "      y_variable: 1\n"
    "    }\n"
    "  }\n"
    "  body {\n"
    "    name: \"Body2\"\n"
    "    position_map {\n"
    "      x_variable: 2\n"
    "      y_variable: 3\n"
    "    }\n"
    "  }\n"
    "}\n";

  Scene scene;
  SceneWrapper::SceneObserver observer = ignoringObserver();
  SceneWrapper wrapper(scene,&observer,"Scene");
  WrapperState state = stateFromText(text);

  wrapper.setState(state);

  assert(stateOf(wrapper)==state);
}


int main()
{
  testHierarchy();
  testAddingBodies();
  testGettingState();
  testSettingStateWithNonEmptyBackgroundFrame();
  testSettingStateWithEmptyBackgroundFrame();
  testSettingStateWithTwoBodies();
}
