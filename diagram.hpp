#ifndef DIAGRAM_HPP_
#define DIAGRAM_HPP_

#include <vector>
#include <memory>
#include "diagramnode.hpp"


class Diagram {
  public:
    using Node = DiagramNode;

    Diagram() = default;
    Diagram(const Diagram &);

    Diagram& operator=(Diagram);

    Node *findNode(NodeIndex);

    NodeIndex addNode(const std::string &text);

    Node &createNode(NodeIndex);
      // This creates a node with a particular index.  This is mainly useful
      // when we are rebuilding diagrams that are read from a file where
      // the nodes have particular indices that we need to preserve.

    void deleteNode(NodeIndex index);

    Node &node(NodeIndex);
    const Node &node(NodeIndex) const;

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
