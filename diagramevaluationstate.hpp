#ifndef DIAGRAMSTATE_HPP_
#define DIAGRAMSTATE_HPP_


#include "any.hpp"
#include "environment.hpp"


struct DiagramEvaluationState {
  struct NodeState {
    std::vector<Any> output_values;
    std::vector<std::string> line_errors;
  };

  DiagramEvaluationState() = default;

  void operator=(const DiagramEvaluationState &) = delete;
  DiagramEvaluationState(const DiagramEvaluationState &) = delete;
  DiagramEvaluationState(DiagramEvaluationState &&) = default;
  DiagramEvaluationState &operator=(DiagramEvaluationState &&) = default;

  std::vector<NodeState> node_states;
  std::vector<std::unique_ptr<Environment>> environment_ptrs;

  Environment &allocateEnvironment(const Environment *parent_environment_ptr);

  void clear();
};


#endif /* DIAGRAMSTATE_HPP_ */
