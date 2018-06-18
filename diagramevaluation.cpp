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
    const Diagram &,
    DiagramState &diagram_state,
    const Node &node,
    NodeIndex node_index,
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
    input_value =
      diagram_state.node_output_values[source_node][source_output_index];
  }

  const Node::Line &line = node.lines[line_index];

  diagram_state.node_output_values[node_index][output_index] =
    evaluateLineText(line.text,vector<float>{input_value},executor);
}


static void
  updateNodeEvaluation(
    const Diagram &diagram,
    DiagramState &diagram_state,
    int node_index,
    vector<bool> &evaluated_flags,
    Executor &executor
  )
{
  assert(node_index>=0);

  if (evaluated_flags[node_index]) {
    return;
  }

  const Node &node = diagram.node(node_index);
  int n_inputs = node.inputs.size();

  for (int i=0; i!=n_inputs; ++i) {
    int source_node_index = node.inputs[i].source_node_index;

    if (source_node_index>=0) {
      updateNodeEvaluation(
        diagram,
        diagram_state,
        source_node_index,
        evaluated_flags,
        executor
      );
    }
  }

  int n_expressions = node.statements.size();
  int next_output_index = 0;
  int next_input_index = 0;
  int n_outputs = node.nOutputs();
  diagram_state.node_output_values[node_index].resize(n_outputs);

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
      diagram_state,
      node,
      node_index,
      i,
      output_index,
      executor,
      source_output_index,
      source_node
    );
  }

  evaluated_flags[node_index] = true;
}


void
  evaluateDiagram(
    const Diagram &diagram,
    Executor &executor,
    DiagramState &diagram_state
  )
{
  int n_nodes = diagram.nNodes();
  vector<bool> evaluated_flags(n_nodes,false);
  diagram_state.node_output_values.resize(n_nodes);

  for (auto i : diagram.existingNodeIndices()) {
    updateNodeEvaluation(diagram,diagram_state,i,evaluated_flags,executor);
  }
}


void evaluateDiagram(const Diagram &diagram,Executor &executor)
{
  DiagramState diagram_state;
  evaluateDiagram(diagram,executor,diagram_state);
}
