#ifndef ABSTRACTDIAGRAMEVALUATOR_HPP_
#define ABSTRACTDIAGRAMEVALUATOR_HPP_

#include "diagramexecutioncontext.hpp"
#include "diagram.hpp"


struct AbstractDiagramEvaluator {
  const DiagramExecutionContext &context;

  AbstractDiagramEvaluator(const DiagramExecutionContext &context_arg)
  : context(context_arg)
  {
  }

  virtual Optional<Any>
    maybeEvaluate(
      const Diagram &diagram,
      const Environment *parent_environment_ptr,
      const Optional<std::string> &optional_expected_type_name = {}
    ) = 0;
};


#endif /* ABSTRACTDIAGRAMEVALUATOR_HPP_ */
