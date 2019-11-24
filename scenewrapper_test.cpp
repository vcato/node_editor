#include "scenewrapper.hpp"

#include <sstream>
#include "streamvector.hpp"
#include "wrapperutil.hpp"
#include "basicvariant.hpp"
#include "wrapperstate.hpp"
#include "makestr.hpp"
#include "treeupdating.hpp"
#include "faketree.hpp"

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
  return result.asValue();
}


static void setWrapperStateFromText(const Wrapper &wrapper,const char *text)
{
  wrapper.setState(stateFromText(text));
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


static void testHierarchy()
{
  Scene scene;
  Scene::Body &body = scene.addBody();
  scene.addChildBodyTo(body);
  SceneWrapper::SceneObserver observer = unusedObserver();
  SceneWrapper wrapper(scene,&observer,"Scene");
  ostringstream stream;
  printStateOn(stream,stateOf(wrapper));
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
  addBodyTo(
    const Wrapper &wrapper,
    const TreePath &path,
    TreeObserver &tree_observer
  )
{
  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &body_wrapper){
      executeAddBodyFunction(body_wrapper,path,tree_observer);
    }
  );
}


static FakeTree makeFakeTree(const Wrapper &wrapper)
{
  FakeTree tree;
  addChildTreeItems(tree, wrapper, TreePath());
  return tree;
}


static string textOfTree(const FakeTree &tree)
{
  ostringstream stream;
  tree.printOn(stream);
  string tree_string = stream.str();
  return tree_string;
}


static bool
   treeMatchesWrapper(const FakeTree &tree,const Wrapper &world_wrapper)
{
  FakeTree new_tree = makeFakeTree(world_wrapper);
  string expected_tree_string = textOfTree(new_tree);
  string tree_string = textOfTree(tree);

  return tree_string == expected_tree_string;
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

  auto ignore_item_removed_function = [](const TreePath &){};
  FakeTree tree = makeFakeTree(scene_wrapper);
  TreeUpdatingObserver
    tree_observer(tree,scene_wrapper,ignore_item_removed_function);

  addBodyTo(scene_wrapper,scene_path,tree_observer);
  addBodyTo(scene_wrapper,first_body_path,tree_observer);
  addBodyTo(scene_wrapper,first_child_body_path,tree_observer);

  assert(scene.nBodies()==1);
  assert(scene.bodies()[0].nChildren()==1);
  assert(scene.bodies()[0].allChildren()[0].nChildren()==1);

  assert(treeMatchesWrapper(tree,scene_wrapper));
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
