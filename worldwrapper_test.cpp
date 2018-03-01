#include "worldwrapper.hpp"

#include <cassert>
#include <string>
#include <sstream>
#include <algorithm>
#include "world.hpp"
#include "wrapperutil.hpp"
#include "streamvector.hpp"


using std::string;
using std::ostringstream;
using std::ostream;
using std::cerr;


namespace {
struct FakeOperationHandler : Wrapper::OperationHandler {
  ostream &command_stream;

  FakeOperationHandler(ostream &command_stream_arg)
  : command_stream(command_stream_arg)
  {
  }

  virtual void addItem(const TreePath &path)
  {
    command_stream << "addItem([" << path << "])\n";
  }

  virtual void replaceTreeItems(const TreePath &)
  {
    assert(false);
  }

  virtual void changeEnumerationValues(const TreePath &path) const
  {
    command_stream << "changeEnumerationValues([" << path << "])\n";
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
struct FakeSceneTree : SceneTree {
  virtual void removeAllItems()
  {
  }

  virtual void setItems(const Item &/*root*/)
  {
  }
};
}


namespace {
struct FakeSceneWindow : SceneWindow {
  FakeSceneViewer viewer_member;
  FakeSceneTree tree_member;

  SceneViewer &viewer() override { return viewer_member; }
  SceneTree &tree() override { return tree_member; }
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


namespace scene_and_charmapper_tests {
static void testAddingABodyToTheScene()
{
  // Create a world with a charmapper and a scene.
  //  Charmapper should have a motion pass with a position expression.
  // Add a body to the scene.
  // Check that the operation handler is called to handle changing the
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
  FakeOperationHandler handler(command_stream);

  executeAddBodyFunction2(world_wrapper,scene_path,handler);

  string command_string = command_stream.str();
  string expected_command_string =
    "addItem([1,0])\n"
    "changeEnumerationValues([0,0,0,0])\n"
    "changeEnumerationValues([0,0,0,2,0])\n";
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
  visitSubWrapper(
    world_wrapper,
    path,
    [&](const Wrapper &wrapper){
      wrapper.accept(
        NumericVisitor(
          [&](const NumericWrapper &numeric_wrapper){
            numeric_wrapper.setValue(value);
          }
        )
      );
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

  pos_expr.target_body = Charmapper::BodyLink(&scene,&body);
  assert(pos_expr.target_body.hasValue());
  TreePath path = makePath(world_wrapper,"Scene|Body|position|x");
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
    Wrapper::OperationHandler &operation_handler
  )
{
  visitEnumeration(
    world_wrapper,
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      int index =
        enumerationLabelIndex(enumeration_wrapper,enumeration_label);
      enumeration_wrapper.setValue(path,index,operation_handler);
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
  FakeOperationHandler operation_handler(command_stream);

  {
    TreePath path = makePath(world_wrapper,"Charmapper|Motion Pass");
    executeOperation2(world_wrapper,path,"Add Pos Expr",operation_handler);
  }

  Charmapper::MotionPass::PosExpr &pos_expr = charmapper.pass(0).expr(0);

  {
    TreePath path =
      makePath(
        world_wrapper,
        "Charmapper|Motion Pass|Pos Expr|Global Position|X"
      );
    setNumericValue(world_wrapper,path,15);
  }

  assert(!pos_expr.hasATargetBody());

  TreePath target_body_path =
    makePath(world_wrapper,"Charmapper|Motion Pass|Pos Expr|Target Body");

  setEnumerationValue(
    world_wrapper,
    target_body_path,
    "scene 0:Body1",
    operation_handler
  );

  assert(pos_expr.target_body.bodyPtr()==&body);
  assert(body.position.x(scene.displayFrame())==15);

  setEnumerationValue(
    world_wrapper,
    target_body_path,
    "None",
    operation_handler
  );

  assert(!pos_expr.target_body.hasValue());
  assert(body.position.x(scene.displayFrame())==0);
}
}


static void setValue(const Wrapper &wrapper,const string &path_string,int value)
{
  TreePath path = makePath(wrapper,path_string);

  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &sub_wrapper){
      sub_wrapper.accept(
        NumericVisitor(
          [&](const NumericWrapper &numeric_wrapper){
            numeric_wrapper.setValue(value);
          }
        )
      );
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
  pos_expr.target_body = Charmapper::BodyLink(&scene,&body);
  WorldWrapper wrapper(world);
  auto &components = charmapper.pass(0).expr(0).global_position.components();

  setValue(wrapper,"Charmapper|Motion Pass|Pos Expr|Global Position|X",15);
  assert(components.x.value==15);
  assert(body.position.x(scene.displayFrame())==15);

  string scene_viewer_commands =
    world.scene_window.viewer_member.command_stream.str();
  assert(scene_viewer_commands=="redrawScene()\n");
  // assert(world.viewer.last_drawn_scene.bodies[0].position.x==15);

  setValue(wrapper,"Charmapper|Motion Pass|Pos Expr|Global Position|Y",20);
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
  pos_expr1.target_body = Charmapper::BodyLink(&scene,&body1);
  pos_expr1.global_position.components().x.value = 1;

  Charmapper &charmapper2 = world.addCharmapper();
  Charmapper::MotionPass &motion_pass2 = charmapper2.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr2 = motion_pass2.addPosExpr();
  pos_expr2.target_body = Charmapper::BodyLink(&scene,&body2);
  pos_expr2.global_position.components().x.value = 2;

  WorldWrapper wrapper(world);

  setValue(wrapper,"Charmapper|Motion Pass|Pos Expr|Global Position|X",3);

  // Make sure both charmaps still have an effect after changing a value
  // in one of them.
  //assert(body2.position.x(scene.displayFrame())==2);
}
}


int main()
{
  {
    namespace tests = scene_and_charmapper_tests;
    tests::testAddingABodyToTheScene();
    tests::testChangingABodyPositionInTheScene();
    tests::testChangingTheTargetBody();
    tests::testUsingCharmapperToMoveABody();
    tests::testWithTwoCharmappers();
  }
}
