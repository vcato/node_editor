#include "diagram.hpp"
#include "executor.hpp"


struct DiagramState {
  std::vector<std::vector<Any>> node_output_values;
};


extern void evaluateDiagram(const Diagram &,Executor &);
extern void evaluateDiagram(const Diagram &,Executor &,DiagramState &);
