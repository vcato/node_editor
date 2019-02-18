#include "evaluatediagram.hpp"

#include <cassert>
#include <sstream>
#include "linetext.hpp"
#include "diagramio.hpp"
#include "anyio.hpp"
#include "evaluatelinetext.hpp"

using std::ostream;
using std::vector;
using std::ostringstream;
using std::cerr;
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
    const vector<Any> &input_values
  )
{
  const Node::Line &line = node.lines[line_index];

  ostringstream line_error_stream;

  Optional<Any> maybe_output_value =
    evaluateLineText(
      line.text,
      input_values,
      executor,
      line_error_stream
      ,
      /* allocate_environment_function */
      [&](const Environment *parent_environment_ptr) -> Environment& {
        return diagram_state.allocateEnvironment(parent_environment_ptr);
      }
    );

#if 0
  cerr << "Evaluated line " << line.text << "\n";
  cerr << "input_values: ";
  printOn(cerr,input_values,0);
  cerr << "\n";
  cerr << "result: " << maybe_output_value << "\n";
#endif

  diagram_state.node_states[node_index].line_errors[line_index] =
    line_error_stream.str();

  if (output_index>=0) {
    if (maybe_output_value) {
      diagram_state.node_states[node_index].output_values[output_index] =
        *maybe_output_value;
    }
    else {
      std::ostream &error_stream = executor.debugStream();
      error_stream << "Error: " << line_error_stream.str() << "\n";
      error_stream << "  line: " << line.text << "\n";

      for (int i=0, n_inputs=input_values.size(); i!=n_inputs; ++i) {
        error_stream << "input_values[" << i << "]=" << input_values[i] << "\n";
      }
    }
  }
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
  int n_lines = node.nLines();
  DiagramState::NodeState &node_state = diagram_state.node_states[node_index];

  assert(node_state.output_values.empty());
  assert(node_state.line_errors.empty());
    // We are assuming that the node state has been cleared before calling
    // this so that we're starting with a clean state.

  node_state.output_values.resize(n_outputs);
  node_state.line_errors.resize(n_lines);

  vector<Any> input_values;

  for (int i=0; i!=n_inputs; ++i) {
    int source_node = node.inputs[i].source_node_index;
    int source_output_index = node.inputs[i].source_output_index;
    Any source_value;

    if (source_node>=0 && source_output_index>=0) {
      source_value =
        diagram_state
        .node_states[source_node]
        .output_values[source_output_index];
    }

    input_values.push_back(source_value);
  }

  for (int i=0; i!=n_expressions; ++i) {
    int output_index = -1;

    if (node.statements[i].has_output) {
      output_index = next_output_index;
      ++next_output_index;
    }

    if (node.lines[i].n_inputs==1) {
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
      input_values
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
  diagram_state.node_states.resize(n_nodes);

  for (auto i : diagram.existingNodeIndices()) {
    updateNodeEvaluation(diagram,diagram_state,i,evaluated_flags,executor);
  }
}


void evaluateDiagram(const Diagram &diagram,Executor &executor)
{
  DiagramState diagram_state;
  evaluateDiagram(diagram,executor,diagram_state);
}
