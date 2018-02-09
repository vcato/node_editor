#include "scene.hpp"

#include <cassert>


namespace {
struct Observer : Scene::Observer {
  int change_count;

  Observer()
  : change_count(0)
  {
  }

  virtual void sceneChanged()
  {
    ++change_count;
  }
};
}


static void testDestroyingObserverBeforeScene()
{
  Scene scene;
  {
    Observer observer;
    observer.setScenePtr(&scene);
    assert(scene.nObservers()==1);
    scene.addBody();
    assert(observer.change_count==1);
  }
  assert(scene.nObservers()==0);
  scene.addBody();
}


#if 1
static void testDestroyingSceneBeforeObserver()
{
  Observer observer;
  {
    Scene scene;
    observer.setScenePtr(&scene);
    assert(scene.nObservers()==1);
  }
}
#endif


int main()
{
  testDestroyingObserverBeforeScene();
  testDestroyingSceneBeforeObserver();
}
