#include <vector>
#include <memory>
#include "node2.hpp"


struct Diagram {
  std::vector<std::unique_ptr<Node2>> _node2s;

  Node2 *findNode(NodeIndex);
  void evaluate();
  void evaluate(std::ostream &);
  void
    updateNodeEvaluation(
      NodeIndex,
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
  NodeIndex addNode(const std::string &text);

  void deleteNode(NodeIndex index);

  Node2 &node(NodeIndex);

  int nExistingNodes() const;

  void
    connectNodes(
      NodeIndex input_node_index,
      int input_index,
      NodeIndex output_node_index,
      int output_index
    );

  void setNodeText(int node_index,const std::string &);

  void removeInvalidInputs();

  std::vector<NodeIndex> existingNodeIndices() const;
};
