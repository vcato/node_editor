#include "testdiagramevaluator.hpp"

#include "diagramexecutor.hpp"
#include "diagramevaluationstate.hpp"
#include "evaluatediagram.hpp"

using std::string;


Optional<Any>
  TestDiagramEvaluator::maybeEvaluate(
    const Diagram &diagram,
    const Environment *parent_environment_ptr,
    const Optional<string> &optional_expected_type_name
  )
{
  DiagramExecutor executor(context,parent_environment_ptr);
  DiagramEvaluationState &diagram_state = diagram_state_map[&diagram];
  executor.optional_expected_return_type_name = optional_expected_type_name;
  evaluateDiagram(diagram,executor,diagram_state);
  return std::move(executor.maybe_return_value);
}
