#include "charmapper.hpp"


static void testWithTargetBody()
{
  Charmapper charmapper;
  Scene scene;
  Scene::Body &body = scene.addBody();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_ptr = &body;
  pos_expr.global_position.components().x.value = 15;
  charmapper.apply();
  assert(body.position.x==15);
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


int main()
{
  testWithTargetBody();
  testWithoutTargetBody();
}
