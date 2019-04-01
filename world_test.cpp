#include "world.hpp"

#include <cstdlib>
#include <sstream>
#include "viewportrect.hpp"
#include "sceneviewerimpl.hpp"
#include "streamvector.hpp"

using std::cerr;
using std::string;
using std::ostringstream;


namespace {
struct Viewer : SceneViewer {
  void redrawScene() override { }

  void userPressesMouseAt(const ViewportPoint &p)
  {
    mousePressedAt(p);
  }

  void userMovesMouseTo(const ViewportPoint &p)
  {
    mouseMovedTo(p);
  }

  void userReleasesMouse()
  {
    mouseReleased();
  }
};
}


namespace {
struct Tree : SceneTree {
  virtual void setItems(const ItemData &/*root*/)
  {
  }

  virtual void insertItem(const std::vector<int> &,const ItemData &)
  {
    assert(false);
  }

  virtual void removeItem(const std::vector<int> &/*path*/)
  {
    assert(false);
  }
};
}


namespace {
struct Window : SceneWindow {
  Viewer viewer_member;
  Tree tree_member;

  SceneViewer &viewer() override { return viewer_member; }
  SceneTree &tree() override { return tree_member; }
  void setTitle(const string &) override { }

  void userPressesMouseAt(ViewportPoint p)
  {
    viewer_member.userPressesMouseAt(p);
  }

  void userMovesMouseTo(ViewportPoint p)
  {
    viewer_member.userMovesMouseTo(p);
  }

  void userReleasesMouse()
  {
    viewer_member.userReleasesMouse();
  }
};
}


namespace {
struct FakeWorld : World {
  using World::World;

  Optional<Window> maybe_window;

  Window &window() { return *maybe_window; }

  bool windowIsCreated() { return maybe_window.hasValue(); }

  SceneWindow& createSceneViewerWindow(SceneMember &) override
  {
    assert(!windowIsCreated());
    maybe_window.emplace();
    return *maybe_window;
  }

  void destroySceneViewerWindow(SceneWindow &window_arg) override
  {
    assert(windowIsCreated());
    assert(&window_arg == &*maybe_window);
    maybe_window.reset();
  }
};
}


namespace {
struct Tester {
  FakeWorld world;
  ostringstream callback_command_stream;

  Tester()
  {
    world.scene_frame_variables_changed_function = [&](
      int scene_member_index,
      int frame_index,
      const std::vector<int> &variable_indices
    ){
      callback_command_stream << "sceneFrameVariablesChanged(" <<
        "scene_member_index=" << scene_member_index << "," <<
        "frame_index=" << frame_index << "," <<
        "variable_indices=" << variable_indices << ")\n";
    };
  }

  string callbackCommandString()
  {
    return callback_command_stream.str();
  }
};
}


static void testAddingAScene()
{
  Tester tester;
  FakeWorld &world = tester.world;
  world.addScene();
}


static ViewportPoint
  centerOfBody(const Scene &scene,const Scene::Body &desired_body)
{
  Optional<ViewportRect> maybe_matching_rect;

  auto match = [&](const Scene::Body &body,const ViewportRect &rect)
  {
    if (&body == &desired_body) {
      maybe_matching_rect = rect;
    }
  };

  scene_viewer::forEachSceneBodyRect(scene,match);

  assert(maybe_matching_rect);

  return maybe_matching_rect->center();
}


static void testMovingABody()
{
  Tester tester;
  // Create add a scene.
  Scene &scene = tester.world.addScene();

  // Add a body to the scene.
  Scene::Body &body = scene.addBody();

  // Click on the body.
  ViewportPoint center_of_body = centerOfBody(scene,body);
  Window &window = tester.world.window();
  window.userPressesMouseAt(center_of_body);

  // Drag to a new location.
  window.userMovesMouseTo(center_of_body + ViewportVector(1,2));

  // Release mouse.
  window.userReleasesMouse();

  // Verify that we got callbacks saying that variables changed.
  string expected_command_string =
    "sceneFrameVariablesChanged("
      "scene_member_index=0,"
      "frame_index=0,"
      "variable_indices=[0,1]"
    ")\n";

  assert(tester.callbackCommandString() == expected_command_string);
}


static void testSceneMemberIndex()
{
  Tester tester;
  tester.world.addScene();
  int index = tester.world.memberIndex(tester.world.sceneMember(0));
  assert(index == 0);
}


int main()
{
  testAddingAScene();
  testSceneMemberIndex();
  testMovingABody();
}
