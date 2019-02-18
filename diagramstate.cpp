#include "diagramstate.hpp"


using std::make_unique;


Environment &
  DiagramState::allocateEnvironment(const Environment *parent_environment_ptr)
{
  environment_ptrs.emplace_back(
    make_unique<Environment>(parent_environment_ptr)
  );

  assert(environment_ptrs.back());
  return *environment_ptrs.back();
}


void DiagramState::clear()
{
  node_states.clear();
  environment_ptrs.clear();
}
