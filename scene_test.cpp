#include "scene.hpp"

#include <cassert>

using std::cerr;


static void testCreatingBodies()
{
  Scene scene;
  assert(scene.nFrameVariables()==0);
  scene.addBody();
  assert(scene.nFrameVariables()==2);
  assert(scene.bodies()[0].name=="Body1");
  scene.addBody();
  assert(scene.bodies()[1].name=="Body2");
  assert(scene.nFrameVariables()==4);
  scene.addChildBodyTo(scene.bodies()[0]);
  assert(scene.bodies()[0].children[0].name=="Body3");
  assert(scene.nFrameVariables()==6);
  assert(scene.backgroundFrame().nVariables()==6);
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
