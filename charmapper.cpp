#include "charmapper.hpp"

#include <iostream>
#include "defaultdiagrams.hpp"


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


static void
  setDisplayedBodyPosition(
    Charmapper::BodyLink &target_body_link,
    const Point2D &new_position
  )
{
  Scene::Body &target_body = target_body_link.body();
  Scene &target_scene = target_body_link.scene();
  Scene::Frame &target_frame = target_scene.displayFrame();
  setBodyPosition(target_body,target_frame,new_position);
}


static Point2D displayedBodyPosition(Charmapper::BodyLink &source_body_link)
{
  Scene::Body &source_body = source_body_link.body();
  Scene &source_scene = source_body_link.scene();
  Scene::Frame &source_frame = source_scene.displayFrame();
  return bodyPosition(source_body,source_frame);
}


static Point2D makePoint2D(const Charmapper::Position &p)
{
  return Point2D(p.x.value,p.y.value);
}


static Vector2D makeVector2D(const Charmapper::Position &p)
{
  return Vector2D(p.x.value,p.y.value);
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

      if (target_body_link.hasValue()) {
        Point2D new_position(0,0);

        if (expr.global_position.isComponents()) {
          new_position = makePoint2D(expr.global_position.components());
        }
        else if (expr.global_position.isFromBody()) {
          auto &from_body_data = expr.global_position.fromBody();
          BodyLink &source_body_link = from_body_data.source_body_link;

          if (source_body_link.hasValue()) {
            new_position = displayedBodyPosition(source_body_link);
          }

          new_position += makeVector2D(from_body_data.local_position);
        }
        else {
          assert(false);
        }

        setDisplayedBodyPosition(target_body_link,new_position);
      }
    }
  }
}


void Charmapper::removePass(int pass_index)
{
  passes.erase(passes.begin()+pass_index);
}
