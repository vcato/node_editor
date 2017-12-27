#include "diagram.hpp"

#include <cassert>


using std::cerr;
using std::vector;
using std::string;
using std::make_unique;
using std::unique_ptr;
using Node = DiagramNode;


Node *Diagram::findNode(NodeIndex i)
{
  return _node_ptrs[i].get();
}


void Diagram::deleteNode(NodeIndex index)
{
  // Disconnect source inputs
  for (const unique_ptr<Node> &node_ptr : _node_ptrs) {
    if (node_ptr) {
      for (auto &input : node_ptr->inputs) {
        if (input.source_node_index==index) {
          input.source_node_index = nullNodeIndex();
        }
      }
    }
  }

  assert(_node_ptrs[index]);
  _node_ptrs[index].reset();
}


NodeIndex Diagram::addNode(const string &text)
{
  NodeIndex node_index = _node_ptrs.size();

  _node_ptrs.emplace_back(make_unique<Node>());
  Node &node = this->node(node_index);
  node.setText(text);
  node.header_text_object.text = "";

  return node_index;
}


Node &Diagram::node(NodeIndex node_index)
{
  assert(_node_ptrs[node_index]);
  return *_node_ptrs[node_index];
}


const Node &Diagram::node(NodeIndex node_index) const
{
  assert(_node_ptrs[node_index]);
  return *_node_ptrs[node_index];
}


void
  Diagram::connectNodes(
    NodeIndex output_node_index,
    int output_index,
    NodeIndex input_node_index,
    int input_index
  )
{
  Node::Input &input = node(input_node_index).inputs[input_index];
  input.source_node_index = output_node_index;
  input.source_output_index = output_index;
}


void Diagram::setNodeText(int node_index,const std::string &text)
{
  node(node_index).setText(text);
  removeInvalidInputs();
}


void Diagram::removeInvalidInputs()
{
  for (NodeIndex i : existingNodeIndices()) {
    int n_inputs = node(i).inputs.size();
    for (int j=0; j!=n_inputs; ++j) {
      Node::Input &input = node(i).inputs[j];
      int source_node_index = input.source_node_index;
      if (source_node_index>=0) {
        Node &source_node = node(source_node_index);
        int n_source_outputs = source_node.outputs.size();
        if (input.source_output_index>=n_source_outputs) {
          input.source_node_index = -1;
          input.source_output_index = -1;
        }
      }
    }
  }
}


vector<NodeIndex> Diagram::existingNodeIndices() const
{
  vector<NodeIndex> result;
  NodeIndex index = 0;
  NodeIndex end = _node_ptrs.size();

  for (;index!=end; ++index) {
    if (_node_ptrs[index]) {
      result.push_back(index);
    }
  }

  return result;
}


int Diagram::nExistingNodes() const
{
  int count = 0;

  for (const unique_ptr<Node> &node_ptr : _node_ptrs) {
    if (node_ptr) {
      ++count;
    }
  }

  return count;
}


int Diagram::nNodes() const
{
  return _node_ptrs.size();
}
