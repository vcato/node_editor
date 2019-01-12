#include "world.hpp"

#include <cstdlib>

using std::string;


namespace {
struct Viewer : SceneViewer {
  void redrawScene() override { }
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
};
}


namespace {
struct FakeWorld : World {
  FakeWorld(Window &window_arg)
  : window(window_arg)
  {
  }

  Window &window;
  bool window_is_created = false;

  SceneWindow& createSceneViewerWindow(SceneMember &) override
  {
    assert(!window_is_created);
    window_is_created = true;
    return window;
  }

  void destroySceneViewerWindow(SceneWindow &window_arg) override
  {
    assert(window_is_created);
    window_is_created = false;
    assert(&window_arg == &window);
  }
};
}


int main()
{
  Window window;
  FakeWorld world(window);
  world.addScene();
}
