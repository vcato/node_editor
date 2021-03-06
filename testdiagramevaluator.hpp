#include "abstractdiagramevaluator.hpp"
#include "diagramevaluationstate.hpp"


struct TestDiagramEvaluator : AbstractDiagramEvaluator {
  std::map<const Diagram *,DiagramEvaluationState> diagram_state_map;

  TestDiagramEvaluator(const DiagramExecutionContext &context)
  : AbstractDiagramEvaluator(context)
  {
  }

  Optional<Any>
    maybeEvaluate(
      const Diagram &diagram,
      const Environment *parent_environment_ptr,
      const Optional<std::string> &optional_expected_return_type_name
    ) override;
};
