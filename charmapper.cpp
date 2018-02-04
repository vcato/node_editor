#include "charmapper.hpp"

#include <iostream>


using std::make_unique;
using std::cerr;


Charmapper::MotionPass::MotionPass()
{
}


void Charmapper::MotionPass::addPosExpr()
{
  pos_exprs.push_back(make_unique<PosExpr>());
}


void Charmapper::addMotionPass()
{
  passes.push_back(make_unique<MotionPass>());
}
