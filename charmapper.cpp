#include "charmapper.hpp"

#include <iostream>
#include "defaultdiagrams.hpp"
#include "removefrom.hpp"
#include "evaluatediagram.hpp"
#include "streamexecutor.hpp"
#include "diagramexecutor.hpp"
#include "diagramio.hpp"
#include "charmapperobjects.hpp"
#include "sceneobjects.hpp"
#include "point2dobject.hpp"
#include "maybepoint2d.hpp"
#include "globalvec.hpp"


using std::make_unique;
using std::cerr;
using std::vector;
using std::unique_ptr;
using MotionPass = Charmapper::MotionPass;



Charmapper::MotionPass::MotionPass()
{
}


template <typename T>
static T& create(vector<unique_ptr<T>> &ptr_vector)
{
  auto ptr = make_unique<T>();
  T &result = *ptr;
  ptr_vector.push_back(std::move(ptr));
  return result;
}

auto Charmapper::MotionPass::addPosExpr() -> PosExpr&
{
  return create<PosExpr>(pos_exprs);
}


MotionPass& Charmapper::addMotionPass()
{
  return create<MotionPass>(passes);
}


Charmapper::GlobalPosition::GlobalPosition()
: diagram(fromComponentsDiagram()),
  global_position_ptr(std::make_unique<ComponentsData>())
{
}


Charmapper::GlobalPosition::FromBodyData::FromBodyData()
: local_position(localPositionDiagram())
{
}


void Charmapper::GlobalPosition::switchToComponents()
{
  diagram = fromComponentsDiagram();
  global_position_ptr = std::make_unique<ComponentsData>();
}


void Charmapper::GlobalPosition::switchToFromBody()
{
  diagram = fromBodyDiagram();
  global_position_ptr = std::make_unique<FromBodyData>();
}


Charmapper::MotionPass::PosExpr::PosExpr()
: diagram(posExprDiagram()),
  local_position(localPositionDiagram())
{
}


void Charmapper::MotionPass::removePosExpr(int index)
{
  removeFrom(pos_exprs,pos_exprs[index]);
}


static void
  setDisplayedBodyPosition(
    BodyLink &target_body_link,
    const Point2D &new_position
  )
{
  Scene::Body &target_body = target_body_link.body();
  Scene &target_scene = target_body_link.scene();
  Scene::Frame &target_frame = target_scene.displayFrame();
  setBodyPosition(target_body,target_frame,new_position);
}


static Point2D displayedBodyPosition(BodyLink &source_body_link)
{
  Scene::Body &source_body = source_body_link.body();
  Scene &source_scene = source_body_link.scene();
  Scene::Frame &source_frame = source_scene.displayFrame();
  return bodyPosition(source_body,source_frame);
}


static Point2D makePoint2D(const Charmapper::GlobalPosition::ComponentsData &p)
{
  return Point2D(p.x.value,p.y.value);
}


static Point2D makePoint2D(const Charmapper::Position &p)
{
  return Point2D(p.x.value,p.y.value);
}


static void
  evaluatePoint2DDiagram(
    Diagram &diagram,
    DiagramExecutor &executor,
    Point2D &new_position
  )
{
  evaluateDiagram(diagram,executor);
  Optional<Point2D> maybe_result = maybePoint2D(executor.return_value);

  if (!maybe_result) {
    return;
  }

  new_position = *maybe_result;
}


void Charmapper::apply()
{
  int n_passes = nPasses();

  for (int i=0; i!=n_passes; ++i) {
    auto &pass = this->pass(i);
    auto n_exprs = pass.nExprs();

    for (int i=0; i!=n_exprs; ++i) {
      auto &expr = pass.expr(i);
      BodyLink &target_body_link = expr.target_body_link;

      // The diagram generates a PosExpr object, which requires a target body.
      if (target_body_link.hasValue()) {
        Point2D global_position(0,0);

        if (expr.global_position.isComponents()) {
          Diagram &diagram = expr.global_position.diagram;
          DiagramExecutor executor(/*show_stream*/cerr,/*error_stream*/cerr);
          Point2D parameters = makePoint2D(expr.global_position.components());
          executor.environment["x"] = parameters.x;
          executor.environment["y"] = parameters.y;
          evaluatePoint2DDiagram(diagram,executor,global_position);
        }
        else if (expr.global_position.isFromBody()) {
          using FromBodyData = GlobalPosition::FromBodyData;
          FromBodyData &from_body_data = expr.global_position.fromBody();
          BodyLink &source_body_link = from_body_data.source_body_link;

          if (source_body_link.hasValue()) {
            global_position = displayedBodyPosition(source_body_link);
          }

          Diagram &diagram = from_body_data.local_position.diagram;
          DiagramExecutor executor(/*show_stream*/cerr,/*error_stream*/cerr);
          float x_param = from_body_data.local_position.x.value;
          float y_param = from_body_data.local_position.y.value;
          executor.environment["x"] = x_param;
          executor.environment["y"] = y_param;
          Point2D local_position(0,0);
          evaluatePoint2DDiagram(diagram,executor,local_position);
          global_position += local_position - Point2D(0,0);
        }
        else {
          assert(false);
        }

        Diagram &diagram = expr.diagram;
        DiagramExecutor executor(/*show_stream*/cerr,/*error_stream*/cerr);
        Class pos_expr_class = posExprClass();
        executor.environment["PosExpr"] = &pos_expr_class;
        executor.environment["target_body"] = bodyObject(target_body_link);
        Point2D local_position = makePoint2D(expr.local_position);
        executor.environment["local_position"] =
          makePoint2DObject(local_position);
        executor.environment["global_position"] =
          makePoint2DObject(global_position);

        evaluateDiagram(diagram,executor);
        Optional<PosExprData> maybe_pos_expr =
          maybePosExpr(executor.return_value);

        if (maybe_pos_expr) {
          setDisplayedBodyPosition(
            maybe_pos_expr->body_link,maybe_pos_expr->position
          );
        }
        else {
          cerr << "pos expr diagram failed\n";
        }
      }
    }
  }
}


void Charmapper::removePass(int pass_index)
{
  passes.erase(passes.begin()+pass_index);
}
