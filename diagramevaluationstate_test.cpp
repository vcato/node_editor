#include "diagramevaluationstate.hpp"


static void testAllocateEnvironment()
{
  DiagramEvaluationState diagram_state;
  const Environment *parent_environment_ptr = nullptr;
  diagram_state.allocateEnvironment(parent_environment_ptr);
  assert(!diagram_state.environment_ptrs.empty());
}


static void testClear()
{
  DiagramEvaluationState diagram_state;
  diagram_state.node_states.resize(1);
  diagram_state.allocateEnvironment(/*parent_environment_ptr*/nullptr);

  diagram_state.clear();

  assert(diagram_state.node_states.empty());
  assert(diagram_state.environment_ptrs.empty());
}


int main()
{
  testAllocateEnvironment();
  testClear();
}
