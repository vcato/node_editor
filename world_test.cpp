#include "world.hpp"

#include <cstdlib>


namespace {
struct Viewer : SceneViewer {
  void redrawScene() override { }
};
}


namespace {
struct Tree : SceneTree {
  virtual void removeAllItems()
  {
    assert(false);
  }

  virtual void setItems(const Item &/*root*/)
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
};
}


namespace {
struct FakeWorld : World {
  FakeWorld(Window &window_arg)
  : window(window_arg)
  {
  }

  Window &window;

  virtual SceneWindow& createSceneViewerWindow(SceneMember &)
  {
    return window;
  }
};
}


int main()
{
  Window window;
  FakeWorld world(window);
  world.addScene();
}
