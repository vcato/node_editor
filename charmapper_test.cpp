#include "charmapper.hpp"

#include <iostream>
#include "testdiagramevaluator.hpp"

using std::cerr;
using std::string;
using std::map;


static map<const Diagram *,DiagramState>
  applyCharmapper(Charmapper &charmapper)
{
  DiagramExecutionContext
    context{/*show_stream*/cerr,/*error_stream*/cerr};
  TestDiagramEvaluator evaluator(context);
  charmapper.apply(evaluator);
  return std::move(evaluator.diagram_state_map);
}


static void testWithTargetBody()
{
  Charmapper charmapper;
  Scene scene;
  Scene::Body &body = scene.addBody();
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link.set(&scene,&body);
  pos_expr.global_position.components().x.value = 15;
  applyCharmapper(charmapper);
  Scene::VarValue result = body.position.x(scene.displayFrame());
  assert(result==15);
}


static void testWithoutTargetBody()
{
  Charmapper charmapper;
  Scene scene;
  Charmapper::MotionPass &motion_pass = charmapper.addMotionPass();
  Charmapper::MotionPass::PosExpr &pos_expr = motion_pass.addPosExpr();
  pos_expr.global_position.components().x.value = 15;
  applyCharmapper(charmapper);
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
  applyCharmapper(charmapper);
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

  // This is setting the display frame.  The display frame is what
  // is used for all calculations.  The background frame is copied
  // to the display frame if World::applyCharmaps() is used, but we're
  // not using that.

  setBodyPosition(body2,scene.displayFrame(),Point2D(15,16));

  applyCharmapper(charmapper);

  Point2D body_position = bodyPosition(body1,scene.displayFrame());
  assert(body_position==Point2D(15,16));
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

  applyCharmapper(charmapper);

  Point2D body1_position = bodyPosition(body1,scene.displayFrame());

  assert(body1_position==Point2D(15+1,16+2));
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

  applyCharmapper(charmapper);

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

  applyCharmapper(charmapper);

  Point2D final_position = bodyPosition(body1,scene.displayFrame());
  Point2D expected_position = Point2D(1-10,2-20);

  if (final_position!=expected_position) {
    cerr << "final_position: " << final_position << "\n";
    cerr << "expected_position: " << expected_position << "\n";
  }

  assert(final_position==expected_position);
}


static void clearDiagram(Diagram &diagram)
{
  for (auto index : diagram.existingNodeIndices()) {
    diagram.deleteNode(index);
  }
}


static void testGlobalPositionDiagram(const string &node_text,float expected_x)
{
  Scene scene;
  auto &body1 = scene.addBody();
  Charmapper charmapper;

  auto &motion_pass = charmapper.addMotionPass();
  auto &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body1);
  pos_expr.global_position.switchToComponents();
  Diagram &diagram = pos_expr.global_position.diagram;
  clearDiagram(diagram);
  diagram.createNodeWithText(node_text);
  applyCharmapper(charmapper);
  assert(body1.position.x(scene.displayFrame())==expected_x);
}


static void testPosExprDiagramWithWrongReturnType()
{
  Scene scene;
  auto &body1 = scene.addBody();
  Charmapper charmapper;

  string expected_error_message = "Returning float instead of PosExpr.\n";

  auto &motion_pass = charmapper.addMotionPass();
  auto &pos_expr = motion_pass.addPosExpr();
  pos_expr.target_body_link = BodyLink(&scene,&body1);
  pos_expr.global_position.switchToComponents();
  Diagram &diagram = pos_expr.diagram;
  clearDiagram(diagram);
  NodeIndex node_index = diagram.createNodeWithText("return 5");

  map<const Diagram *,DiagramState> diagram_state_map =
    applyCharmapper(charmapper);

  DiagramState &diagram_state = diagram_state_map[&diagram];
  int line_index = 0;
  string error_message =
    diagram_state.node_states[node_index].line_errors[line_index];

  if (error_message != expected_error_message) {
    cerr << "error_message: " << error_message << "\n";
  }

  assert(error_message == expected_error_message);
}


int main()
{
  testWithTargetBody();
  testWithoutTargetBody();
  testWithFrame();
  testFromSourceBody();
  testFromSourceBodyWithLocalOffset();
  testFromSourceBodyWithLocalOffsetAndNoSourceBody();
  testTargetLocalOffset();
  testGlobalPositionDiagram("return [1,2]",/*expected_x*/1);
  testGlobalPositionDiagram("return [1,2",/*expected_x*/0);
  testGlobalPositionDiagram("return [1,2,3]",/*expected_x*/0);
  testGlobalPositionDiagram("return [[],2]",/*expected_x*/0);
  testGlobalPositionDiagram("return [1,[]]",/*expected_x*/0);
  testPosExprDiagramWithWrongReturnType();
}
