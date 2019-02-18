#include "evaluatediagram.hpp"

#include <cassert>
#include <sstream>
#include "linetext.hpp"
#include "diagramio.hpp"
#include "anyio.hpp"
#include "evaluatestatement.hpp"

using std::ostream;
using std::string;
using std::vector;
using std::ostringstream;
using std::cerr;
using Node = DiagramNode;


namespace {
struct LineRange {
  const int begin_line_index;
  const int end_line_index;
};
}


static LineRange
  statementLineRange(const Node &node,int desired_statement_index)
{
  int line_index = 0;
  int statement_index = 0;

  for (;;) {
    int n_lines_in_statement = node.statements[statement_index].n_lines;
    int next_line_index = line_index + n_lines_in_statement;

    if (statement_index == desired_statement_index) {
      return LineRange{line_index,next_line_index};
    }

    ++statement_index;
    line_index = next_line_index;
  }
}


static string
  buildStatementString(const Node &node,int desired_statement_index)
{
  LineRange line_range = statementLineRange(node,desired_statement_index);
  int line_index = line_range.begin_line_index;
  int next_line_index = line_range.end_line_index;

  string statement = "";

  while (line_index != next_line_index) {
    statement += node.lines[line_index].text + " ";
    ++line_index;
  }

  return statement;
}


static void
  evaluateDiagramNodeStatement(
    const Diagram &,
    DiagramState &diagram_state,
    const Node &node,
    DiagramState::NodeState &node_state,
    int statement_index,
    int output_index,
    Executor &executor,
    const vector<Any> &input_values
  )
{
  string statement = buildStatementString(node,statement_index);

  ostringstream statement_error_stream;

  auto allocate_environment_function =
    [&](const Environment *parent_environment_ptr) -> Environment& {
      return diagram_state.allocateEnvironment(parent_environment_ptr);
    };

  Optional<Any> maybe_output_value =
    evaluateStatement(
      statement,
      input_values,
      executor,
      statement_error_stream,
      allocate_environment_function
    );

#if 0
  cerr << "Evaluated line " << line.text << "\n";
  cerr << "input_values: ";
  printOn(cerr,input_values,0);
  cerr << "\n";
  cerr << "result: " << maybe_output_value << "\n";
#endif

  {
    LineRange line_range = statementLineRange(node,statement_index);
    int begin = line_range.begin_line_index;
    int end = line_range.end_line_index;

    for (int line_index = begin; line_index != end; ++line_index) {
      node_state.line_errors[line_index] = statement_error_stream.str();
    }
  }

  if (output_index>=0) {
    if (maybe_output_value) {
      node_state.output_values[output_index] = *maybe_output_value;
    }
    else {
      std::ostream &error_stream = executor.debugStream();
      error_stream << "Error: " << statement_error_stream.str() << "\n";
      error_stream << "  statement: " << statement << "\n";

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
  );


static void
  evaluateNode(
    const Diagram &diagram,
    DiagramState &diagram_state,
    const Node &node,
    DiagramState::NodeState &node_state,
    vector<bool> &evaluated_flags,
    Executor &executor
  )
{
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

  int n_statements = node.statements.size();
  int next_output_index = 0;
  int n_outputs = node.nOutputs();
  int n_lines = node.nLines();

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

  for (
    int statement_index=0;
    statement_index!=n_statements;
    ++statement_index
  ) {
    int output_index = -1;

    if (node.statements[statement_index].has_output) {
      output_index = next_output_index;
      ++next_output_index;
    }

    evaluateDiagramNodeStatement(
      diagram,
      diagram_state,
      node,
      node_state,
      statement_index,
      output_index,
      executor,
      input_values
    );
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

  evaluateNode(
    diagram,
    diagram_state,
    diagram.node(node_index),
    diagram_state.node_states[node_index],
    evaluated_flags,
    executor
  );

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
