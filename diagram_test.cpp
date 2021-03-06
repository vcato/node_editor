#include "diagram.hpp"

#include <cassert>
#include <sstream>
#include "evaluatediagram.hpp"
#include "fakeexecutor.hpp"


using std::ostringstream;
using std::ostream;
using std::string;
using std::cerr;




static void
  evaluateDiagram(Diagram &diagram,DiagramEvaluationState &diagram_state)
{
  ostringstream output_stream;
  ostringstream execution_stream;
  FakeExecutor
    executor(
      /*parent_environment_ptr*/ nullptr,
      execution_stream,
      output_stream,
      /*debug_stream*/cerr
    );
  evaluateDiagram(diagram,executor,diagram_state);
}


static void testEvaluation1()
{
  Diagram diagram;
  int node_index =  diagram.createNodeWithText("5");
  DiagramEvaluationState diagram_state;
  evaluateDiagram(diagram,diagram_state);
  assert(diagram_state.node_states[node_index].output_values[0]==5);
}


static void testEvaluation2()
{
  Diagram diagram;
  int node_index =  diagram.createNodeWithText("6");
  DiagramEvaluationState diagram_state;
  evaluateDiagram(diagram,diagram_state);
  assert(diagram_state.node_states[node_index].output_values[0]==6);
}


static void evaluate(Diagram &diagram,ostream &output_stream)
{
  ostringstream debug_stream;
  ostringstream execution_stream;
  FakeExecutor
    executor(
      /*parent_environment_ptr*/nullptr,
      execution_stream,
      output_stream,
      debug_stream
    );
  DiagramEvaluationState diagram_state;
  evaluateDiagram(diagram,executor,diagram_state);
}


static void testEvaluateShow()
{
  Diagram diagram;
  diagram.createNodeWithText("show(5)");
  ostringstream stream;
  evaluate(diagram,stream);
  string output = stream.str();
  assert(output=="5\n");
}


static void testEvaluateEmpty()
{
  Diagram diagram;
  diagram.createNodeWithText("");
  ostringstream stream;
  evaluate(diagram,stream);
}


static void testEvaluateShowDisconnectedInput()
{
  Diagram diagram;
  diagram.createNodeWithText("show($)");
  ostringstream stream;
  evaluate(diagram,stream);
}


static void testEvaluateShowConnectedInput()
{
  Diagram diagram;
  int n1 = diagram.createNodeWithText("5");
  int n2 = diagram.createNodeWithText("show($)");
  int input_node_index = n2;
  int input_index = 0;
  int output_node_index = n1;
  int output_index = 0;
  diagram.connectNodes(
    output_node_index,output_index,
    input_node_index,input_index
  );
  ostringstream stream;
  evaluate(diagram,stream);
  string output = stream.str();
  assert(output=="5\n");
}


static void testEvaluateShowConnectedInput2()
{
  Diagram diagram;
  int n1 = diagram.createNodeWithText("5");
  int n2 = diagram.createNodeWithText("show($)");
  int input_node_index = n2;
  int input_index = 0;
  int output_node_index = n1;
  int output_index = 0;
  diagram.connectNodes(
    output_node_index,output_index,
    input_node_index,input_index
  );
  diagram.setNodeText(n1,"");
  assert(diagram.node(n1).nOutputs()==0);
  assert(diagram.node(n2).inputs[0].source_node_index<0);
}


static void testEvaluateAfterDeletingAnInput()
{
  Diagram diagram;
  int n1 = diagram.createNodeWithText("5");
  int n2 = diagram.createNodeWithText("b=$");
  diagram.connectNodes(n1,0,n2,0);
  diagram.deleteNode(n1);
  assert(diagram.findNode(n2));
  assert(diagram.node(n2).inputs[0].source_node_index==nullNodeIndex());
  ostringstream output_stream;
  diagram.removeInvalidInputs();
  evaluate(diagram,output_stream);

  // Just making sure the evaluate doesn't crash
}


static void testExistingNodeIndices()
{
  Diagram diagram;
  assert(diagram.existingNodeIndices().size()==0);
}


static void testEvaluatingIncompleteVectorOverTwoLines()
{
  Diagram diagram;
  diagram.createNodeWithText("[1,2\n");

  ostringstream stream;
  evaluate(diagram,stream);
}


static void testCopy()
{
  Diagram d1;
  Diagram d2(d1);
}


static void testAssign()
{
  Diagram d1;
  Diagram d2;
  d2 = d1;
}


int main()
{
  testEvaluation1();
  testEvaluation2();
  testEvaluateShow();
  testEvaluateEmpty();
  testEvaluateShowDisconnectedInput();
  testEvaluateShowConnectedInput();
  testEvaluateShowConnectedInput2();
  testEvaluateAfterDeletingAnInput();
  testExistingNodeIndices();
  testEvaluatingIncompleteVectorOverTwoLines();
  testCopy();
  testAssign();
}
