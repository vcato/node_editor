#include "scenewrapper.hpp"

#include <sstream>
#include "streamvector.hpp"
#include "wrapperutil.hpp"
#include "basicvariant.hpp"
#include "wrapperstate.hpp"

using std::ostringstream;
using std::ostream;
using std::string;
using std::cerr;
using std::vector;


static void printTree(ostream &stream,const Wrapper &wrapper,int indent = 0)
{
  printStateOn(stream,stateOf(wrapper),indent);
}


static SceneWrapper::SceneObserver unusedObserver()
{
  return
    SceneWrapper::SceneObserver(
      [](const Wrapper::TreeObserver &){
        assert(false);
      }
    );
}


static void testHierarchy()
{
  Scene scene;
  Scene::Body &body = scene.addBody();
  scene.addChildBodyTo(body);
  SceneWrapper wrapper(scene,unusedObserver(),"Scene");
  ostringstream stream;
  printTree(stream,wrapper);
  string output = stream.str();

  auto expected_output =
    "scene {\n"
    "  background_frame {\n"
    "    0: 0\n"
    "    1: 0\n"
    "    2: 0\n"
    "    3: 0\n"
    "  }\n"
    "  body {\n"
    "    name: \"Body1\"\n"
    "    position {\n"
    "      x: 0\n"
    "      y: 0\n"
    "    }\n"
    "    body {\n"
    "      name: \"Body2\"\n"
    "      position {\n"
    "        x: 0\n"
    "        y: 0\n"
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

    virtual void itemAdded(const TreePath &path)
    {
      stream << "addItem: path=" << path << "\n";
    }

    virtual void itemReplaced(const TreePath &)
    {
      assert(false);
    }

    virtual void enumarationValuesChanged(const TreePath &) const
    {
      assert(false);
    }

    virtual void itemRemoved(const TreePath &)
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
  SceneWrapper::SceneObserver notify([](const Wrapper::TreeObserver &){});
  SceneWrapper wrapper(scene,notify,"Scene");
  ostringstream stream;

  int body_index = 2;
  addBodyTo(wrapper,{},stream);
  addBodyTo(wrapper,{1},stream);
  addBodyTo(wrapper,{1,body_index},stream);

  assert(scene.nBodies()==1);
  assert(scene.bodies()[0].nChildren()==1);
  assert(scene.bodies()[0].children[0].nChildren()==1);

  string body_index_str = std::to_string(body_index);
  string commands = stream.str();
  string expected_commands =
    "addItem: path=[0,0]\n"
    "addItem: path=[0,1]\n"
    "addItem: path=[1]\n"
    "addItem: path=[0,2]\n"
    "addItem: path=[0,3]\n"
    "addItem: path=[1," + body_index_str + "]\n"
    "addItem: path=[0,4]\n"
    "addItem: path=[0,5]\n"
    "addItem: path=[1," + body_index_str + "," + body_index_str + "]\n";

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
    [](const Wrapper::TreeObserver &){ assert(false); }
  );
  Scene::Body &body = scene.addBody();
  scene.addChildBodyTo(body);
  SceneWrapper wrapper(scene,notify,"Scene");
  WrapperState state = stateOf(wrapper);
  ostringstream stream;
  printStateOn(stream,state);
  string output = stream.str();

  auto expected_output =
    "scene {\n"
    "  background_frame {\n"
    "    0: 0\n"
    "    1: 0\n"
    "    2: 0\n"
    "    3: 0\n"
    "  }\n"
    "  body {\n"
    "    name: \"Body1\"\n"
    "    position {\n"
    "      x: 0\n"
    "      y: 0\n"
    "    }\n"
    "    body {\n"
    "      name: \"Body2\"\n"
    "      position {\n"
    "        x: 0\n"
    "        y: 0\n"
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


#if 0
static void testBuildingFromState()
{
  Scene scene;
  SceneWrapper wrapper(scene,unusedObserver(),"Scene");
  WrapperState state;
  state.label = "Scene";
  wrapper.setState(state);
  assert(stateOf(wrapper)==state);
}
#endif


int main()
{
  testHierarchy();
  testAddingBodies();
  testGettingState();
  // testBuildingFromState();
}
