#include "diagramevaluation.hpp"

#include <cassert>
#include <sstream>
#include "linetext.hpp"

using std::ostream;
using std::vector;
using std::ostringstream;
using Node = DiagramNode;


static void
  evaluateDiagramNodeLine(
    const Diagram &diagram,
    Node &node,
    int line_index,
    int output_index,
    Executor &executor,
    int source_output_index,
    int source_node
  )
{
  if (output_index<0) {
    return;
  }

  float input_value = 0;

  if (source_node>=0) {
    input_value = diagram.node(source_node).outputs[source_output_index].value;
  }

  const Node::Line &line = node.lines[line_index];
  Node::Output &output = node.outputs[output_index];

  output.value = evaluateLineText(line.text,vector<float>{input_value},executor);
}


static void
  updateNodeEvaluation(
    Diagram &diagram,
    int node_index,
    vector<bool> &evaluated_flags,
    Executor &executor
  )
{
  assert(node_index>=0);

  if (evaluated_flags[node_index]) {
    return;
  }

  Node &node = diagram.node(node_index);
  int n_inputs = node.inputs.size();

  for (int i=0; i!=n_inputs; ++i) {
    int source_node_index = node.inputs[i].source_node_index;

    if (source_node_index>=0) {
      updateNodeEvaluation(
        diagram,source_node_index,evaluated_flags,executor
      );
    }
  }

  int n_expressions = node.statements.size();
  int next_output_index = 0;
  int next_input_index = 0;

  for (int i=0; i!=n_expressions; ++i) {
    int source_output_index = -1;
    int source_node = -1;
    int output_index = -1;

    if (node.statements[i].has_output) {
      output_index = next_output_index;
      ++next_output_index;
    }

    if (node.lines[i].n_inputs==1) {
      source_node = node.inputs[next_input_index].source_node_index;
      source_output_index = node.inputs[next_input_index].source_output_index;
      next_input_index += node.lines[i].n_inputs;
    }

    evaluateDiagramNodeLine(
      diagram,
      node,
      i,
      output_index,
      executor,
      source_output_index,
      source_node
    );
  }

  evaluated_flags[node_index] = true;
}


void evaluateDiagram(Diagram &diagram,Executor &executor)
{
  int n_nodes = diagram.nNodes();
  vector<bool> evaluated_flags(n_nodes,false);

  for (auto i : diagram.existingNodeIndices()) {
    updateNodeEvaluation(diagram,i,evaluated_flags,executor);
  }
}
