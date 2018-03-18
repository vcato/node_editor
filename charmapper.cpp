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


void Charmapper::apply()
{
  int n_passes = nPasses();

  for (int i=0; i!=n_passes; ++i) {
    auto &pass = this->pass(i);
    auto n_exprs = pass.nExprs();
    for (int i=0; i!=n_exprs; ++i) {
      auto &expr = pass.expr(i);
      if (expr.global_position.isComponents()) {
        if (expr.target_body.hasValue()) {
          Scene::Point2DMap &position = expr.target_body.body().position;
          Scene &scene = expr.target_body.scene();
          const Position &global_position = expr.global_position.components();
          position.x.set(scene.displayFrame(),global_position.x.value);
          position.y.set(scene.displayFrame(),global_position.y.value);
        }
      }
      else if (expr.global_position.isFromBody()) {
        if (expr.target_body.hasValue()) {
          assert(false);
        }
      }
      else {
        assert(false);
      }
    }
  }
}


void Charmapper::removePass(int pass_index)
{
  passes.erase(passes.begin()+pass_index);
}
