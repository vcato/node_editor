#include "worldwrapper.hpp"

#include <cassert>
#include <string>
#include <sstream>
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

  virtual void redrawScene()
  {
    command_stream << "redrawScene()\n";
  }
};
}

namespace {
struct FakeWorld : World {
  FakeSceneViewer viewer;

  SceneViewer& createSceneViewerWindow(SceneMember &) override
  {
    return viewer;
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


namespace scene_and_charmapper_tests {
static void testChangingABodyPositionInTheScene()
{
  FakeWorld world;
  Charmapper &charmapper = world.addCharmapper();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  motion_pass.addPosExpr();
  Scene &scene = world.addScene(); // child 1
  scene.addBody(); // child 0
  WorldWrapper world_wrapper(world);
  visitSubWrapper(
    world_wrapper,
    {/*scene*/1,/*body*/0,/*position*/1,/*x*/0},
    [](const Wrapper &sub_wrapper){
      sub_wrapper.accept(NumericVisitor(
        [](const NumericWrapper &x_wrapper){
          x_wrapper.setValue(1);
        }
      ));
    }
  );

  // Just making sure it doesn't crash.
}
}


namespace scene_and_charmapper_tests {
static void testTargetBodyPtr()
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

  assert(!charmapper.pass(0).expr(0).hasATargetBody());

  {
    TreePath path =
      makePath(world_wrapper,"Charmapper|Motion Pass|Pos Expr|Target Body");
    visitSubWrapper(world_wrapper,path,[&](const Wrapper &wrapper){
      wrapper.accept(
        EnumerationVisitor(
          [&](const EnumerationWrapper &enumeration_wrapper){
            enumeration_wrapper.setValue(
              path,/*index*/1,operation_handler
            );
          }
        )
      );
    });
  }
  {
    Body *target_body_ptr = charmapper.pass(0).expr(0).target_body_ptr;
    assert(target_body_ptr);
    assert(target_body_ptr==&body);
  }
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
  pos_expr.setTargetBodyPtr(&body);
  WorldWrapper wrapper(world);
  auto &components = charmapper.pass(0).expr(0).global_position.components();

  setValue(wrapper,"Charmapper|Motion Pass|Pos Expr|Global Position|X",15);
  assert(components.x.value==15);
  assert(body.position.x==15);

  string scene_viewer_commands = world.viewer.command_stream.str();
  assert(scene_viewer_commands=="redrawScene()\n");

  setValue(wrapper,"Charmapper|Motion Pass|Pos Expr|Global Position|Y",20);
  assert(components.y.value==20);
  assert(body.position.y==20);
}
}


int main()
{
  {
    namespace tests = scene_and_charmapper_tests;
    tests::testAddingABodyToTheScene();
    tests::testChangingABodyPositionInTheScene();
    tests::testTargetBodyPtr();
    tests::testUsingCharmapperToMoveABody();
  }
}
