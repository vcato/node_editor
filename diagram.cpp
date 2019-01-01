#include "diagram.hpp"

#include <cassert>
#include <iostream>


using std::cerr;
using std::vector;
using std::string;
using std::make_unique;
using std::unique_ptr;
using Node = DiagramNode;



template <typename NodeVector>
static void copyNodes(const NodeVector &from,NodeVector &to)
{
  assert(to.empty());

  to.resize(from.size());

  for (typename NodeVector::size_type i=0, n=from.size(); i!=n; ++i) {
    if (from[i]) {
      to[i] = make_unique<DiagramNode>(*from[i]);
    }
  }
}


Diagram::Diagram(const Diagram &arg)
{
  copyNodes(arg._node_ptrs,_node_ptrs);
}


Diagram& Diagram::operator=(Diagram arg)
{
  swap(_node_ptrs,arg._node_ptrs);

  return *this;
}


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


NodeIndex Diagram::createNode()
{
  NodeIndex node_index = _node_ptrs.size();

  _node_ptrs.emplace_back(make_unique<Node>());

  assert(node(node_index).header_text_object.text == "");

  return node_index;
}


NodeIndex Diagram::createNodeWithText(const string &text)
{
  NodeIndex node_index = createNode();

  node(node_index).setText(text);

  return node_index;
}


DiagramNode &Diagram::createNode(NodeIndex node_index)
{
  assert(node_index>=0);
  int n_nodes = _node_ptrs.size();

  if (node_index>=n_nodes) {
    _node_ptrs.resize(node_index+1);
  }

  assert(!_node_ptrs[node_index]);

  _node_ptrs[node_index] = make_unique<Node>();

  return *_node_ptrs[node_index];
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


NodeIndex Diagram::createDuplicateOfNode(NodeIndex n)
{
  NodeIndex new_node_index = createNode();
  node(new_node_index) = node(n);
  return new_node_index;
}


vector<NodeIndex>
  Diagram::duplicateNodes(
    const vector<NodeIndex> &indices_of_nodes_to_duplicate
  )
{
  vector<NodeIndex> new_node_indices;

  for (NodeIndex index_of_node_to_duplicate : indices_of_nodes_to_duplicate) {
    NodeIndex new_node_index =
      createDuplicateOfNode(index_of_node_to_duplicate);
    new_node_indices.push_back(new_node_index);
  }

  return new_node_indices;
}
