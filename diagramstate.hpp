#ifndef DIAGRAMSTATE_HPP_
#define DIAGRAMSTATE_HPP_


#include "any.hpp"


struct DiagramState {
  struct NodeState {
    std::vector<Any> output_values;
    std::vector<std::string> line_errors;
  };

  std::vector<NodeState> node_states;
};


#endif /* DIAGRAMSTATE_HPP_ */
