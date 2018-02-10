#include "world.hpp"

#include <cstdlib>


namespace {
struct Window : SceneViewer {
  virtual void sceneChanged()
  {
  }
};
}


namespace {
struct FakeWorld : World {
  FakeWorld(Window &window_arg)
  : window(window_arg)
  {
  }

  Window &window;

  virtual SceneViewer& createSceneViewerWindow(SceneMember &)
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
