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
