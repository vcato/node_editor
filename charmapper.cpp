#include "charmapper.hpp"

#include <iostream>


using std::make_unique;
using std::cerr;
using MotionPass = Charmapper::MotionPass;


Charmapper::MotionPass::MotionPass()
{
}


void Charmapper::MotionPass::addPosExpr()
{
  pos_exprs.push_back(make_unique<PosExpr>());
}


MotionPass& Charmapper::addMotionPass()
{
  auto motion_pass_ptr = make_unique<MotionPass>();
  MotionPass &motion_pass = *motion_pass_ptr;
  passes.push_back(std::move(motion_pass_ptr));
  return motion_pass;
}
