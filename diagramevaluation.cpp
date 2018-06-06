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
    Diagram &diagram,
    Node &node,
    int line_index,
    int output_index,
    ostream &stream,
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

  Node::Line &line = node.lines[line_index];
  Node::Output &output = node.outputs[output_index];

  StreamExecutor executor = {stream};
  evaluateLineText(line.text,vector<float>{input_value},executor);
  output.value = executor.output_value;
}


static void
  updateNodeEvaluation(
    Diagram &diagram,
    int node_index,
    vector<bool> &evaluated_flags,
    ostream &stream
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
      updateNodeEvaluation(diagram,source_node_index,evaluated_flags,stream);
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
      diagram,node,i,output_index,stream,source_output_index,source_node
    );
  }

  evaluated_flags[node_index] = true;
}


void evaluateDiagram(Diagram &diagram,ostream &stream)
{
  int n_nodes = diagram.nNodes();
  vector<bool> evaluated_flags(n_nodes,false);

  for (auto i : diagram.existingNodeIndices()) {
    updateNodeEvaluation(diagram,i,evaluated_flags,stream);
  }
}


void evaluateDiagram(Diagram &diagram)
{
  ostringstream dummy_stream;
  evaluateDiagram(diagram,dummy_stream);
}
