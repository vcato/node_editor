#include "diagramevaluationstate.hpp"


using std::make_unique;


Environment &
  DiagramEvaluationState::allocateEnvironment(
    const Environment *parent_environment_ptr
  )
{
  environment_ptrs.emplace_back(
    make_unique<Environment>(parent_environment_ptr)
  );

  assert(environment_ptrs.back());
  return *environment_ptrs.back();
}


void DiagramEvaluationState::clear()
{
  node_states.clear();
  environment_ptrs.clear();
}
