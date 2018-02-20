#include "scene.hpp"

#include <cassert>

using std::cerr;


static void testCreatingBodies()
{
  Scene scene;
  scene.addBody();
  assert(scene.bodies()[0].name=="Body1");
  scene.addBody();
  assert(scene.bodies()[1].name=="Body2");
  scene.addChildBodyTo(scene.bodies()[0]);
  assert(scene.bodies()[0].children[0].name=="Body3");
}


static void testCopyingScene()
{
  Scene a;
  Scene b = a;
}


int main()
{
  testCreatingBodies();
  testCopyingScene();
}
