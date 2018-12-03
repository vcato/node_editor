#include "scene.hpp"

#include <cassert>
#include <iostream>
#include "bodylink.hpp"
#include "streamvector.hpp"


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
  assert(scene.bodies()[0].child(0).name=="Body3");
  assert(scene.nFrameVariables()==6);
  assert(scene.backgroundFrame().nVariables()==6);
}


static void testCopyingScene()
{
  Scene a;
  Scene b = a;
}


static void testParentBody()
{
  Scene scene;
  Scene::Body &parent = scene.addBody();
  Scene::Body &child = scene.addChildBodyTo(parent);
  assert(child.parentPtr()==&parent);
}


static void testGlobalPos()
{
  Scene scene;
  Scene::Body &body = scene.addBody();
  Point2D result =
    globalPos(
      body,
      /*local*/Point2D(0,0),
      /*frame*/scene.backgroundFrame()
    );
  assert(result==Point2D(0,0));
}


static void testGlobalPos2()
{
  Scene scene;
  Scene::Body &body1 = scene.addBody();
  Scene::Body &body2 = scene.addChildBodyTo(body1);
  Scene::Frame &frame = scene.backgroundFrame();
  setBodyPosition(body1,frame,Point2D(10,0));
  setBodyPosition(body2,frame,Point2D(0,15));
  Point2D result =
    globalPos(
      body2,
      /*local*/Point2D(1,2),
      /*frame*/scene.backgroundFrame()
    );
  assert(result==Point2D(10 + 1,15 + 2));
}


int main()
{
  testCreatingBodies();
  testCopyingScene();
  testParentBody();
  testGlobalPos();
  testGlobalPos2();
}
