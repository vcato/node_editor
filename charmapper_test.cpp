#include "charmapper.hpp"

#include <iostream>

using BodyLink = Charmapper::BodyLink;
using std::cerr;


static void testWithTargetBody()
{
  Charmapper charmapper;
  Scene scene;
  Scene::Body &body = scene.addBody();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link.set(&scene,&body);
  pos_expr.global_position.components().x.value = 15;
  charmapper.apply();
  assert(body.position.x(scene.displayFrame())==15);
}


static void testWithoutTargetBody()
{
  Charmapper charmapper;
  Scene scene;
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.global_position.components().x.value = 15;
  charmapper.apply();
}


static void testWithFrame()
{
  Scene scene;
  auto &body = scene.addBody();

  Charmapper charmapper;
  auto& motion_pass = charmapper.addMotionPass();
  auto& pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body);
  pos_expr.global_position.components().x.value = 15;

  body.position.x.set(scene.backgroundFrame(),0);
  charmapper.apply();
  assert(body.position.x(scene.displayFrame()) == 15);
}


static void testFromSourceBody()
{
  Scene scene;
  auto &body1 = scene.addBody();
  auto &body2 = scene.addBody();

  Charmapper charmapper;
  auto &motion_pass = charmapper.addMotionPass();
  auto &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body1);
  pos_expr.global_position.switchToFromBody();
  pos_expr.global_position.fromBody().source_body_link.set(&scene,&body2);

  setBodyPosition(body2,scene.displayFrame(),Point2D(15,16));

  charmapper.apply();

  assert(bodyPosition(body1,scene.displayFrame())==Point2D(15,16));
}


static void testFromSourceBodyWithLocalOffset()
{
  Scene scene;
  auto &body1 = scene.addBody();
  auto &body2 = scene.addBody();

  Charmapper charmapper;
  auto &motion_pass = charmapper.addMotionPass();
  auto &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body1);
  pos_expr.global_position.switchToFromBody();
  pos_expr.global_position.fromBody().source_body_link.set(&scene,&body2);
  pos_expr.global_position.fromBody().local_position.x.value = 1;
  pos_expr.global_position.fromBody().local_position.y.value = 2;

  setBodyPosition(body2,scene.displayFrame(),Point2D(15,16));

  charmapper.apply();

  assert(bodyPosition(body1,scene.displayFrame())==Point2D(15+1,16+2));
}


static void testFromSourceBodyWithLocalOffsetAndNoSourceBody()
{
  Scene scene;
  auto &body1 = scene.addBody();
  auto &body2 = scene.addBody();

  Charmapper charmapper;
  auto &motion_pass = charmapper.addMotionPass();
  auto &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body1);
  pos_expr.global_position.switchToFromBody();
  pos_expr.global_position.fromBody().local_position.x.value = 1;
  pos_expr.global_position.fromBody().local_position.y.value = 2;

  setBodyPosition(body2,scene.displayFrame(),Point2D(15,16));

  charmapper.apply();

  assert(bodyPosition(body1,scene.displayFrame())==Point2D(1,2));
}


static void testTargetLocalOffset()
{
  Scene scene;
  auto &body1 = scene.addBody();

  Charmapper charmapper;
  auto &motion_pass = charmapper.addMotionPass();
  auto &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body1);
  pos_expr.local_position.x.value = 10;
  pos_expr.local_position.y.value = 20;
  pos_expr.global_position.switchToComponents();
  pos_expr.global_position.components().x.value = 1;
  pos_expr.global_position.components().y.value = 2;

  charmapper.apply();

  Point2D final_position = bodyPosition(body1,scene.displayFrame());
  assert(final_position==Point2D(1-10,2-20));
}


#if 0
static void clearDiagram(Diagram &diagram)
{
  for (auto index : diagram.existingNodeIndices()) {
    diagram.deleteNode(index);
  }
}


static void testDiagram()
{
  Scene scene;
  auto &body1 = scene.addBody();
  Charmapper charmapper;

  auto &motion_pass = charmapper.addMotionPass();
  auto &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body1);
  pos_expr.global_position.switchToComponents();
  Diagram &diagram = pos_expr.global_position.components().diagram;
  clearDiagram(diagram);
  diagram.addNode("return [1,2]");
  cerr << "--- applying charmapper\n";
  charmapper.apply();
  assert(body1.position.x(scene.displayFrame())==1);
}
#endif


int main()
{
  testWithTargetBody();
  testWithoutTargetBody();
  testWithFrame();
  testFromSourceBody();
  testFromSourceBodyWithLocalOffset();
  testFromSourceBodyWithLocalOffsetAndNoSourceBody();
  testTargetLocalOffset();
  // testDiagram();
}
