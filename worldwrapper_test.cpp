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


static void testAddingABodyToTheSceneWithACharmapper()
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

  Wrapper::PerformOperationFunction add_body_function =
    findAddBodyFunction2(world_wrapper,scene_path);

  ostringstream command_stream;
  FakeOperationHandler handler(command_stream);

  add_body_function(handler);

  string command_string = command_stream.str();
  string expected_command_string =
    "addItem([1,0])\n"
    "changeEnumerationValues([0,0,0,0])\n"
    "changeEnumerationValues([0,0,0,2,0])\n";
  assert(command_string==expected_command_string);
}


int main()
{
  testAddingABodyToTheSceneWithACharmapper();
}
