#include "charmapper.hpp"


using BodyLink = Charmapper::BodyLink;


static void testWithTargetBody()
{
  Charmapper charmapper;
  Scene scene;
  Scene::Body &body = scene.addBody();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body.body_ptr = &body;
  pos_expr.global_position.components().x.value = 15;
#if USE_FRAMES
  Scene::Frame frame = scene.makeFrame();
  charmapper.apply(frame);
  assert(body.position.x(frame)==15);
#else
  charmapper.apply();
  assert(body.position.x==15);
#endif
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
  /*auto frame =*/ scene.makeFrame();

  Charmapper charmapper;
  auto& motion_pass = charmapper.addMotionPass();
  auto& pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body = BodyLink(&scene,&body);
  pos_expr.global_position.components().x.value = 15;

#if USE_FRAMES
  frame.var_values[body.position.x.var_index] = 0;
  charmapper.apply(scene);
  assert(frame.var_values[body.position.x.value] == 15);
#endif
}


int main()
{
  testWithTargetBody();
  testWithoutTargetBody();
  testWithFrame();
}
