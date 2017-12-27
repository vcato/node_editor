#ifndef DIAGRAM_HPP_
#define DIAGRAM_HPP_

#include <vector>
#include <memory>
#include "diagramnode.hpp"


class Diagram {
  public:
    using Node = DiagramNode;

    Node *findNode(NodeIndex);

    NodeIndex addNode(const std::string &text);

    void deleteNode(NodeIndex index);

    Node &node(NodeIndex);

    int nExistingNodes() const;
    int nNodes() const;

    void
      connectNodes(
        NodeIndex output_node_index,
        int output_index,
        NodeIndex input_node_index,
        int input_index
      );

    void setNodeText(int node_index,const std::string &);

    void removeInvalidInputs();

    std::vector<NodeIndex> existingNodeIndices() const;

  private:
    std::vector<std::unique_ptr<Node>> _node_ptrs;
};

#endif /* DIAGRAM_HPP_ */
