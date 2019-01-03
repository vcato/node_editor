#include "abstractdiagramevaluator.hpp"


struct DiagramEvaluator : AbstractDiagramEvaluator {
  DiagramEvaluator(const DiagramExecutionContext &context)
  : AbstractDiagramEvaluator(context)
  {
  }

  Optional<Any>
    maybeEvaluate(
      const Diagram &diagram,
      const Environment *parent_environment_ptr
    ) override;
};
