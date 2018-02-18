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
  virtual void redrawScene()
  {
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
static void testUsingCharmapperToMoveABody()
{
  FakeWorld world;

  // Add a charmapper
  Charmapper &charmapper = world.addCharmapper();
  // add a scene
  Scene &scene = world.addScene();
  // add a body to the scene
  Scene::Body &body = scene.addBody();
  // add motion pass to charmapper
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  // add a pos expr to the motion pass
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  // set the target body to the body in the scene
  pos_expr.target_body_ptr = &body;
  // set the x value of the global position to 15
  WorldWrapper wrapper(world);
#if 0
  TreePath path =
    makePath(wrapper,"charmapper|motion pass|pos expr|global position|x");
  visitSubWrapper(
    wrapper,
    path,
    [](const Wrapper &sub_wrapper){
      sub_wrapper.accept(
        NumericVisitor([](const NumericWrapper &numeric_wrapper){
          numeric_wrapper.setValue(15);
        })
      )
    }
  );
#endif

  // check that the body has a position of 15.
}
}


int main()
{
  {
    namespace tests = scene_and_charmapper_tests;
    tests::testAddingABodyToTheScene();
    tests::testChangingABodyPositionInTheScene();
    tests::testUsingCharmapperToMoveABody();
  }
}
