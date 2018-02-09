#include "world.hpp"

#include <cstdlib>


namespace {
struct Window : Scene::Observer {
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

  virtual void createSceneWindow(Scene &scene)
  {
    window.setScenePtr(&scene);
  }
};
}


int main()
{
  Window window;
  FakeWorld world(window);
  world.addScene();
}
