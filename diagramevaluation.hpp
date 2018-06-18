#include <iostream>
#include "diagram.hpp"
#include "executor.hpp"


struct DiagramState {
#if 1
  std::vector<std::vector<float>> node_output_values;
#else
  std::vector<std::vector<Any>> node_output_values;
#endif
};


extern void evaluateDiagram(const Diagram &,Executor &);
extern void evaluateDiagram(const Diagram &,Executor &,DiagramState &);
