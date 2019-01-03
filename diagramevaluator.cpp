#include "diagramevaluator.hpp"

#include "diagramexecutor.hpp"
#include "diagramstate.hpp"
#include "evaluatediagram.hpp"


Optional<Any>
  DiagramEvaluator::maybeEvaluate(
    const Diagram &diagram,
    const Environment *parent_environment_ptr
  )
{
  DiagramExecutor executor(context,parent_environment_ptr);
  DiagramState diagram_state;
  evaluateDiagram(diagram,executor,diagram_state);
  return std::move(executor.maybe_return_value);
}
