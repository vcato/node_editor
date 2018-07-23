#include "worldwrapper.hpp"

#include <cassert>
#include <string>
#include <sstream>
#include <algorithm>
#include "world.hpp"
#include "wrapperutil.hpp"
#include "streamvector.hpp"
#include "diagramio.hpp"
#include "wrapperstate.hpp"
#include "scenewrapper.hpp"


using std::string;
using std::ostringstream;
using std::ostream;
using std::cerr;


static NodeIndex nodeIndexWithText(const Diagram &diagram,const string &text)
{
  for (NodeIndex node_index : diagram.existingNodeIndices()) {
    if (diagram.node(node_index).text()==text) {
      return node_index;
    }
  }

  assert(false);
}


static void
  replaceNodeText(
    Diagram &diagram,
    const string &old_text,
    const string &new_text
  )
{
  NodeIndex node_index = nodeIndexWithText(diagram,old_text);
  diagram.node(node_index).setText(new_text);
}


static void notifyDiagramChanged(Wrapper &wrapper,const string &path_string)
{
  TreePath path = makePath(wrapper,path_string);
  WrapperVisitor visitor =
    [](const Wrapper &sub_wrapper){ sub_wrapper.diagramChanged(); };
  visitSubWrapper(wrapper,path,visitor);
}


namespace {
struct FakeTreeObserver : Wrapper::TreeObserver {
  ostream &command_stream;

  FakeTreeObserver(ostream &command_stream_arg)
  : command_stream(command_stream_arg)
  {
  }

  virtual void itemAdded(const TreePath &path)
  {
    command_stream << "addItem(" << path << ")\n";
  }

  virtual void itemReplaced(const TreePath &)
  {
    assert(false);
  }

  virtual void enumarationValuesChanged(const TreePath &path) const
  {
    command_stream << "changeEnumerationValues(" << path << ")\n";
  }

  virtual void itemRemoved(const TreePath &path)
  {
    command_stream << "removeItem(" << path << ")\n";
  }
};
}


namespace {
struct FakeSceneViewer : SceneViewer {
  ostringstream command_stream;
  Scene last_drawn_scene;

  virtual void redrawScene()
  {
    command_stream << "redrawScene()\n";
    // last_drawn_scene = *scenePtr();
  }
};
}


namespace {
struct StubSceneTree : SceneTree {
  virtual void removeAllItems()
  {
  }

  virtual void setItems(const ItemData &/*root*/)
  {
  }

  virtual void insertItem(const std::vector<int> &,const ItemData &)
  {
  }

  virtual void removeItem(const std::vector<int> &/*path*/)
  {
  }
};
}


namespace {
struct FakeSceneWindow : SceneWindow {
  FakeSceneViewer viewer_member;
  StubSceneTree tree_member;

  SceneViewer &viewer() override { return viewer_member; }
  SceneTree &tree() override { return tree_member; }
  void setTitle(const std::string &) override { }
};
}

namespace {
struct FakeWorld : World {
  FakeSceneWindow scene_window;

  SceneWindow& createSceneViewerWindow(SceneMember &) override
  {
    return scene_window;
  }
};
}


namespace {
struct TreeObserverStub : Wrapper::TreeObserver {
  void itemAdded(const TreePath &) override
  {
    assert(false);
  }

  void itemReplaced(const TreePath &) override
  {
    assert(false);
  }

  void itemRemoved(const TreePath &) override
  {
    assert(false);
  }

  void enumarationValuesChanged(const TreePath &) const override
  {
    assert(false);
  }
};
}


static void testAddingACharmapper()
{
  FakeWorld world;
  WorldWrapper wrapper(world);
  world.addCharmapper();
  wrapper.withChildWrapper(0,[](const Wrapper &child_wrapper){
    assert(child_wrapper.label()=="Charmapper1");
  });
}


static void testPrintingState()
{
  FakeWorld world;
  WorldWrapper wrapper(world);
  ostringstream stream;
  printStateOn(stream,stateOf(wrapper));
  string text = stream.str();
  assert(text=="world\n");
}


static void testPrintingCharmapperState()
{
  FakeWorld world;
  Charmapper &charmapper = world.addCharmapper();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  /*Charmapper::MotionPass::PosExpr &pos_expr =*/ motion_pass.addPosExpr();
  WorldWrapper wrapper(world);
  ostringstream stream;
  WrapperState state = stateOf(wrapper);
  assert(state.children.size()==1);
  printStateOn(stream,state);
  string text = stream.str();
  const char *expected_text =
    "world {\n"
    "  charmapper1 {\n"
    "    motion_pass {\n"
    "      pos_expr {\n"
    "        target_body: none\n"
    "        local_position {\n"
    "          x: 0\n"
    "          y: 0\n"
    "        }\n"
    "        global_position: components {\n"
    "          x: 0\n"
    "          y: 0\n"
    "        }\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n";

  if (text!=expected_text) {
    cerr << "text: " << text << "\n";
  }

  assert(text==expected_text);
}


namespace scene_and_charmapper_tests {
static void testAddingABodyToTheScene()
{
  // Create a world with a charmapper and a scene.
  //  Charmapper should have a motion pass with a position expression.
  // Add a body to the scene.
  // Check that the tree observer is called to handle changing the
  // enumeration values in the source body.

  FakeWorld world;
  Charmapper &charmapper = world.addCharmapper();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.global_position.switchToFromBody();

  world.addScene();
  WorldWrapper world_wrapper(world);
  TreePath scene_path = {1};

  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);

  executeAddBodyFunction2(world_wrapper,scene_path,tree_observer);

  string command_string = command_stream.str();
  string expected_command_string =
    "addItem([1,0,0])\n"
    "addItem([1,0,1])\n"
    "addItem([1,1])\n"
    "changeEnumerationValues([0,0,0,0])\n"
    "changeEnumerationValues([0,0,0,2,0])\n";

  if (command_string!=expected_command_string) {
    cerr << "command_string:\n";
    cerr << command_string << "\n";
  }

  assert(command_string==expected_command_string);
}
}


namespace scene_and_charmapper_tests {
static void testAddingABodyToABody()
{
  FakeWorld world;
  Scene &scene = world.addScene();
  scene.addBody();
  WorldWrapper world_wrapper(world);
  TreePath scene_path = {0};

  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);

  executeOperation(world_wrapper,"Scene1|Body","Add Body",tree_observer);

  string command_string = command_stream.str();
  string expected_command_string =
    "addItem([0,0,2])\n"
    "addItem([0,0,3])\n"
    "addItem([0,1,2])\n";

  if (command_string!=expected_command_string) {
    cerr << "command_string:\n";
    cerr << command_string << "\n";
  }

  assert(command_string==expected_command_string);
}
}


namespace {
static void
  setNumericValue(
    const Wrapper &world_wrapper,
    const TreePath &path,
    int value
  )
{
  visitNumericSubWrapper(
    world_wrapper,
    path,
    [&](const NumericWrapper &numeric_wrapper){
      numeric_wrapper.setValue(value);
    }
  );
}
}


namespace scene_and_charmapper_tests {
static void testChangingABodyPositionInTheScene()
{
  FakeWorld world;
  Charmapper &charmapper = world.addCharmapper();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  Scene &scene = world.addScene(); // child 1
  Scene::Body &body = scene.addBody(); // child 0
  WorldWrapper world_wrapper(world);

  pos_expr.target_body_link = Charmapper::BodyLink(&scene,&body);
  assert(pos_expr.target_body_link.hasValue());
  TreePath path = makePath(world_wrapper,"Scene1|Body|position map|x variable");
  setNumericValue(world_wrapper,path,1);

  // The charmap should override this value
  assert(body.position.x(scene.displayFrame())==0);
}
}


namespace {
static int
  enumerationLabelIndex(const EnumerationWrapper &wrapper,const string &label)
{
  auto names = wrapper.enumerationNames();
  auto iter = std::find(names.begin(),names.end(),label);
  if (iter==names.end()) {
    cerr << "Can't find enumeration label " << label << "\n";
    cerr << "labels: " << names << "\n";
  }
  assert(iter!=names.end());
  return iter-names.begin();
}
}


static void
  setEnumerationValue(
    const Wrapper &world_wrapper,
    const TreePath &path,
    const string &enumeration_label,
    Wrapper::TreeObserver &tree_observer
  )
{
  visitEnumerationSubWrapper(
    world_wrapper,
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      int index =
        enumerationLabelIndex(enumeration_wrapper,enumeration_label);
      enumeration_wrapper.setValue(path,index,tree_observer);
    }
  );
}


namespace scene_and_charmapper_tests {
static void testChangingTheTargetBody()
{
  using Body = Scene::Body;
  FakeWorld world;

  Charmapper &charmapper = world.addCharmapper();
  Scene &scene = world.addScene();
  Body &body = scene.addBody();
  charmapper.addMotionPass();
  WorldWrapper world_wrapper(world);
  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);
  string motion_pass_path = "Charmapper1|Motion Pass";

  {
    TreePath path = makePath(world_wrapper,motion_pass_path);
    executeOperation2(world_wrapper,path,"Add Pos Expr",tree_observer);
  }

  Charmapper::MotionPass::PosExpr &pos_expr = charmapper.pass(0).expr(0);

  {
    TreePath path =
      makePath(
        world_wrapper,
        motion_pass_path + "|Pos Expr|Global Position|X"
      );
    setNumericValue(world_wrapper,path,15);
  }

  assert(!pos_expr.hasATargetBody());

  TreePath target_body_path =
    makePath(world_wrapper,motion_pass_path + "|Pos Expr|Target Body");

  setEnumerationValue(
    world_wrapper,
    target_body_path,
    "Scene1:Body1",
    tree_observer
  );

  assert(pos_expr.target_body_link.bodyPtr()==&body);
  assert(body.position.x(scene.displayFrame())==15);

  setEnumerationValue(
    world_wrapper,
    target_body_path,
    "None",
    tree_observer
  );

  assert(!pos_expr.target_body_link.hasValue());
  assert(body.position.x(scene.displayFrame())==0);
}
}


static void setValue(const Wrapper &wrapper,const string &path_string,int value)
{
  TreePath path = makePath(wrapper,path_string);

  visitNumericSubWrapper(
    wrapper,
    path,
    [&](const NumericWrapper &numeric_wrapper){
      numeric_wrapper.setValue(value);
    }
  );
}


namespace scene_and_charmapper_tests {
static void testUsingCharmapperToMoveABody()
{
  FakeWorld world;

  Charmapper &charmapper = world.addCharmapper();
  Scene &scene = world.addScene();
  Scene::Body &body = scene.addBody();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = Charmapper::BodyLink(&scene,&body);
  WorldWrapper wrapper(world);
  auto &components = charmapper.pass(0).expr(0).global_position.components();
  string global_position_path =
    "Charmapper1|Motion Pass|Pos Expr|Global Position";

  setValue(wrapper,global_position_path + "|X",15);
  assert(components.x.value==15);
  assert(body.position.x(scene.displayFrame())==15);

  string scene_viewer_commands =
    world.scene_window.viewer_member.command_stream.str();
  assert(scene_viewer_commands=="redrawScene()\n");

  setValue(wrapper,global_position_path + "|Y",20);
  assert(components.y.value==20);
  assert(body.position.y(scene.displayFrame())==20);
}
}


namespace scene_and_charmapper_tests {
static void testWithTwoCharmappers()
{
  FakeWorld world;

  Scene &scene = world.addScene();
  Scene::Body &body1 = scene.addBody();
  Scene::Body &body2 = scene.addBody();

  Charmapper &charmapper1 = world.addCharmapper();
  Charmapper::MotionPass &motion_pass1 = charmapper1.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr1 = motion_pass1.addPosExpr();
  pos_expr1.target_body_link = Charmapper::BodyLink(&scene,&body1);
  pos_expr1.global_position.components().x.value = 1;

  Charmapper &charmapper2 = world.addCharmapper();
  Charmapper::MotionPass &motion_pass2 = charmapper2.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr2 = motion_pass2.addPosExpr();
  pos_expr2.target_body_link = Charmapper::BodyLink(&scene,&body2);
  pos_expr2.global_position.components().x.value = 2;

  WorldWrapper wrapper(world);

  setValue(wrapper,"Charmapper1|Motion Pass|Pos Expr|Global Position|X",3);
  assert(pos_expr1.global_position.components().x.value==3);

  // Make sure both charmaps still have an effect after changing a value
  // in one of them.
  float x_position_of_body_2 = body2.position.x(scene.displayFrame());
  assert(x_position_of_body_2==2);
}
}


namespace scene_and_charmapper_tests {
static void testRemovingABodyFromTheScene()
{
  // Create a world with a charmapper and a scene with one body.
  // Charmapper should have a motion pass with a position expression.
  // Remove the body from the scene.
  // Check that the tree observer is called to handle changing the
  // enumeration values in the target body.

  FakeWorld world;
  Charmapper &charmapper = world.addCharmapper();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  /*Charmapper::MotionPass::PosExpr &pos_expr =*/ motion_pass.addPosExpr();

  Scene &scene = world.addScene();
  scene.addBody();
  WorldWrapper world_wrapper(world);
  TreePath scene_path = {1};
  TreePath body_path = join(scene_path,SceneWrapper::firstBodyIndex());

  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);

  executeOperation2(world_wrapper,body_path,"Remove",tree_observer);

  string command_string = command_stream.str();
  string expected_command_string =
    "removeItem([1,1])\n"
    "changeEnumerationValues([0,0,0,0])\n"
    ;

  if (command_string!=expected_command_string) {
    cerr << "command_string:\n";
    cerr << command_string << "\n";
  }

  assert(command_string==expected_command_string);
}


static void testRemovingAPosExprFromAMotionPass()
{
  FakeWorld world;
  Charmapper &charmapper = world.addCharmapper();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  motion_pass.addPosExpr();
  WorldWrapper world_wrapper(world);
  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);

  executeOperation(
    world_wrapper,
    "Charmapper1|Motion Pass|Pos Expr","Remove",
    tree_observer
  );

  assert(motion_pass.nExprs()==0);
  string command_string = command_stream.str();
  string expected_command_string = "removeItem([0,0,0])\n";
  assert(command_string==expected_command_string);
}


static void testRemovingACharmapper()
{
  FakeWorld world;
  world.addCharmapper();
  WorldWrapper world_wrapper(world);
  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);
  executeOperation(world_wrapper,"Charmapper1","Remove",tree_observer);
  assert(world.nMembers()==0);
  string expected_command_string = "removeItem([0])\n";
  string command_string = command_stream.str();
  assert(command_string==expected_command_string);
}


static void testChangingGlobalPositionDiagram()
{
  FakeWorld world;
  Charmapper &charmapper = world.addCharmapper();
  Scene &scene = world.addScene();
  Scene::Body &body = scene.addBody();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link.set(&scene,&body);
  Diagram &diagram = pos_expr.global_position.diagram;
  replaceNodeText(diagram,"return $\n","return [1,2]");
  WorldWrapper world_wrapper(world);
  {
    float x = body.position.x(scene.displayFrame());
    assert(x==0);
  }
  string path_string = "Charmapper1|Motion Pass|Pos Expr|Global Position";
  notifyDiagramChanged(world_wrapper,path_string);
  {
    float x = body.position.x(scene.displayFrame());
    assert(x==1);
  }
}

}


static void testSettingEmptyState()
{
  FakeWorld world;
  WorldWrapper wrapper(world);
  WrapperState state = stateOf(wrapper);
  TreeObserverStub tree_observer;
  wrapper.setState(state,TreePath(),tree_observer);
  assert(world.nMembers()==0);
}


static void testSettingStateWithScene()
{
  FakeWorld world;
  world.addScene();
  WorldWrapper wrapper(world);
  WrapperState state = stateOf(wrapper);
  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);

  FakeWorld world2;
  WorldWrapper(world2).setState(state,TreePath(),tree_observer);
  assert(world2.nMembers()==1);
  world2.sceneMember(0);
  string commands = command_stream.str();
  assert(commands=="addItem([0])\n");
}


static void testSettingStateWithSceneWithBody()
{
  FakeWorld world;
  Scene &scene = world.addScene();
  scene.addBody();

  WorldWrapper wrapper(world);
  WrapperState state = stateOf(wrapper);
  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);

  FakeWorld world2;
  WorldWrapper(world2).setState(state,TreePath(),tree_observer);
  assert(world2.nMembers()==1);
  world2.sceneMember(0);
  string commands = command_stream.str();
  assert(commands=="addItem([0])\n");
}


static void testSettingStateWithCharmapper()
{
  FakeWorld world;
  world.addCharmapper();

  WorldWrapper wrapper(world);
  WrapperState state = stateOf(wrapper);
  ostringstream command_stream;
  FakeTreeObserver tree_observer(command_stream);

  FakeWorld world2;
  WorldWrapper(world2).setState(state,TreePath(),tree_observer);
  assert(world2.nMembers()==1);
  world2.charmapperMember(0);
  string commands = command_stream.str();
  assert(commands=="addItem([0])\n");
}


int main()
{
  testAddingACharmapper();
  testPrintingState();
  testPrintingCharmapperState();
  testSettingEmptyState();
  testSettingStateWithScene();
  testSettingStateWithSceneWithBody();
  testSettingStateWithCharmapper();

  {
    namespace tests = scene_and_charmapper_tests;
    tests::testAddingABodyToTheScene();
    tests::testAddingABodyToABody();
    tests::testChangingABodyPositionInTheScene();
    tests::testChangingTheTargetBody();
    tests::testUsingCharmapperToMoveABody();
    tests::testWithTwoCharmappers();
    tests::testRemovingABodyFromTheScene();
    tests::testRemovingAPosExprFromAMotionPass();
    tests::testRemovingACharmapper();
    tests::testChangingGlobalPositionDiagram();
  }
}
