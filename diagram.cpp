#include "diagram.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

using std::cerr;
using std::vector;
using std::string;
using std::ostream;
using std::ostringstream;


void
  Diagram::evaluateLine(
    Node2 &node,int line_index,int output_index,ostream &stream,
    int source_output_index,int source_node
  )
{
  if (output_index<0) {
    return;
  }

  float input_value = 0;

  if (source_node>=0) {
    input_value = _node2s[source_node].outputs[source_output_index].value;
  }

  Node2::Line &line = node.lines[line_index];
  Node2::Output &output = node.outputs[output_index];

  output.value = lineTextValue(line.text,stream,input_value);
}


void
  Diagram::updateNodeEvaluation(
    int node_index,
    vector<bool> &evaluated_flags,
    ostream &stream
  )
{
  assert(node_index>=0);

  if (evaluated_flags[node_index]) return;
  Node2 &node = this->node(node_index);
  int n_inputs = node.inputs.size();

  for (int i=0; i!=n_inputs; ++i) {
    int source_node_index = node.inputs[i].source_node_index;
    if (source_node_index>=0) {
      updateNodeEvaluation(source_node_index,evaluated_flags,stream);
    }
  }

  int n_lines = node.lines.size();
  int next_output_index = 0;
  int next_input_index = 0;

  for (int i=0; i!=n_lines; ++i) {
    int source_output_index = -1;
    int source_node = -1;
    int output_index = -1;
    if (node.lines[i].has_output) {
      output_index = next_output_index;
      ++next_output_index;
    }
    if (node.lines[i].has_input) {
      source_node = node.inputs[next_input_index].source_node_index;
      source_output_index = node.inputs[next_input_index].source_output_index;
      ++next_input_index;
    }
    evaluateLine(node,i,output_index,stream,source_output_index,source_node);
  }

  evaluated_flags[node_index] = true;
}


void Diagram::evaluate()
{
  ostringstream dummy_stream;
  evaluate(dummy_stream);
}


void Diagram::evaluate(ostream &stream)
{
  cerr << "In Diagram::evaluate()\n";

  int n_nodes = _node2s.size();

  vector<bool> evaluated_flags(n_nodes,false);

  for (int i=0; i!=n_nodes; ++i) {
    updateNodeEvaluation(i,evaluated_flags,stream);
  }
}


int Diagram::addNode(const string &text)
{
  int node_index = _node2s.size();

  _node2s.emplace_back();
  Node2 &node = _node2s[node_index];
  node.setText(text);
  node.header_text_object.text = "";

  return node_index;
}


Node2 &Diagram::node(int node_index)
{
  return _node2s[node_index];
}


void
  Diagram::connectNodes(
    int input_node_index,
    int input_index,
    int output_node_index,
    int output_index
  )
{
  Node2::Input &input = _node2s[input_node_index].inputs[input_index];
  input.source_node_index = output_node_index;
  input.source_output_index = output_index;
}


void Diagram::setNodeText(int node_index,const std::string &text)
{
  _node2s[node_index].setText(text);
  removeInvalidInputs();
}


void Diagram::removeInvalidInputs()
{
  int n_nodes = _node2s.size();

  for (int i=0; i!=n_nodes; ++i) {
    int n_inputs = _node2s[i].inputs.size();
    for (int j=0; j!=n_inputs; ++j) {
      Node2::Input &input = _node2s[i].inputs[j];
      int source_node_index = input.source_node_index;
      if (source_node_index>=0) {
        if (input.source_node_index>=n_nodes) {
          assert(false);
        }
        Node2 &source_node = _node2s[source_node_index];
        int n_source_outputs = source_node.outputs.size();
        if (input.source_output_index>=n_source_outputs) {
          input.source_node_index = -1;
          input.source_output_index = -1;
        }
      }
    }
  }
}
