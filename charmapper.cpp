#include "charmapper.hpp"

#include "defaultdiagrams.hpp"
#include "removefrom.hpp"
#include "diagramexecutor.hpp"
#include "evaluatediagram.hpp"
#include "maybepoint2d.hpp"
#include "sceneobjects.hpp"
#include "point2dobject.hpp"
#include "charmapperobjects.hpp"

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


const Diagram&
  Charmapper::GlobalPosition::defaultComponentsDiagram()
{
  return fromComponentsDiagram();
}


const Diagram &
  Charmapper::GlobalPosition::defaultFromBodyDiagram()
{
  return fromBodyDiagram();
}


Charmapper::GlobalPosition::GlobalPosition()
: diagram(defaultComponentsDiagram()),
  global_position_ptr(std::make_unique<ComponentsData>())
{
}


const Diagram&
  Charmapper::GlobalPosition::FromBodyData::defaultLocalPositionDiagram()
{
  return localPositionDiagram();
}


Charmapper::GlobalPosition::FromBodyData::FromBodyData()
: local_position(defaultLocalPositionDiagram())
{
}


void Charmapper::GlobalPosition::switchToComponents()
{
  diagram = defaultComponentsDiagram();
  global_position_ptr = std::make_unique<ComponentsData>();
}


void Charmapper::GlobalPosition::switchToFromBody()
{
  diagram = defaultFromBodyDiagram();
  global_position_ptr = std::make_unique<FromBodyData>();
}


const Diagram &Charmapper::MotionPass::PosExpr::defaultLocalPositionDiagram()
{
  return localPositionDiagram();
}


const Diagram&
  Charmapper::MotionPass::PosExpr::defaultDiagram()
{
  return posExprDiagram();
}


Charmapper::MotionPass::PosExpr::PosExpr()
: diagram(defaultDiagram()),
  local_position(defaultLocalPositionDiagram())
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
    const Environment *parent_environment_ptr,
    Point2D &new_position,
    AbstractDiagramEvaluator &evaluator
  )
{
  Optional<Any> maybe_return_value =
    evaluator.maybeEvaluate(diagram,parent_environment_ptr);

  if (!maybe_return_value) {
    return;
  }

  Optional<Point2D> maybe_result = maybePoint2D(*maybe_return_value);

  if (!maybe_result) {
    return;
  }

  new_position = *maybe_result;
}


void Charmapper::apply(AbstractDiagramEvaluator &evaluator)
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
          Point2D parameters = makePoint2D(expr.global_position.components());

          Environment environment(evaluator.context.parent_environment_ptr);
          environment["x"] = parameters.x;
          environment["y"] = parameters.y;
          evaluatePoint2DDiagram(
            diagram,&environment,global_position,evaluator
          );
        }
        else if (expr.global_position.isFromBody()) {
          using FromBodyData = GlobalPosition::FromBodyData;
          FromBodyData &from_body_data = expr.global_position.fromBody();
          BodyLink &source_body_link = from_body_data.source_body_link;


          Diagram &local_position_diagram =
            from_body_data.local_position.diagram;
          Point2D local_position(0,0);
          {
            float x_param = from_body_data.local_position.x.value;
            float y_param = from_body_data.local_position.y.value;

            Environment environment(evaluator.context.parent_environment_ptr);
            environment["x"] = x_param;
            environment["y"] = y_param;

            evaluatePoint2DDiagram(
              local_position_diagram,&environment,local_position,
              evaluator
            );
          }
          {
            Environment environment(evaluator.context.parent_environment_ptr);
            environment["source_body"] = makeBodyObject(source_body_link);
            environment["local_position"] = makePoint2DObject(local_position);

            evaluatePoint2DDiagram(
              expr.global_position.diagram,
              &environment,
              global_position,
              evaluator
            );
          }
        }
        else {
          assert(false);
        }

        Diagram &diagram = expr.diagram;
        Class pos_expr_class = posExprClass();
        Point2D local_position = makePoint2D(expr.local_position);

        Environment environment(evaluator.context.parent_environment_ptr);
        environment["PosExpr"] = &pos_expr_class;
        environment["target_body"] = makeBodyObject(target_body_link);
        environment["local_position"] = makePoint2DObject(local_position);
        environment["global_position"] = makePoint2DObject(global_position);

        Optional<Any> maybe_return_value =
          evaluator.maybeEvaluate(
            diagram,
            &environment,
            PosExprObjectData::staticTypeName()
          );

        Optional<PosExprData> maybe_pos_expr;

        if (maybe_return_value) {
          maybe_pos_expr =
            maybePosExpr(*maybe_return_value,evaluator.context.error_stream);
        }
        else {
          evaluator.context.error_stream << "Diagram did not return anything\n";
        }

        if (maybe_pos_expr) {
          setDisplayedBodyPosition(
            maybe_pos_expr->body_link,maybe_pos_expr->position
          );
        }
        else {
          evaluator.context.error_stream << "pos expr diagram failed\n";
        }
      }
    }
  }
}


void Charmapper::removePass(int pass_index)
{
  passes.erase(passes.begin()+pass_index);
}
