#include <vector>
#include "node2.hpp"

struct Diagram {
  std::vector<Node2> _node2s;

  void evaluate();
  void evaluate(std::ostream &);
  void
    updateNodeEvaluation(
      int node_index,
      std::vector<bool> &evaluated_flags,
      std::ostream &
    );
  void
    evaluateLine(
      Node2 &node,
      int line_index,
      int output_index,
      std::ostream &,
      int input_index,
      int input_node
    );
  int addNode(const std::string &text);
  Node2 &node(int node_index);

  void
    connectNodes(
      int input_node_index,
      int input_index,
      int output_node_index,
      int output_index
    );

  void setNodeText(int node_index,const std::string &);

  void removeInvalidInputs();
};
