#include "diagram.hpp"

#include <cassert>
#include <sstream>
#include "diagramevaluation.hpp"


using std::ostringstream;
using std::ostream;
using std::string;
using std::cerr;


static void testEvaluation1()
{
  Diagram diagram;
  int node_index =  diagram.addNode("5");
  evaluateDiagram(diagram);
  assert(diagram.node(node_index).outputs[0].value==5);
}


static void testEvaluation2()
{
  Diagram diagram;
  int node_index =  diagram.addNode("6");
  evaluateDiagram(diagram);
  assert(diagram.node(node_index).outputs[0].value==6);
}


static void evaluate(Diagram &diagram,ostream &stream)
{
  evaluateDiagram(diagram,stream);
}


static void testEvaluateShow()
{
  Diagram diagram;
  diagram.addNode("show(5)");
  ostringstream stream;
  evaluate(diagram,stream);
  string output = stream.str();
  assert(output=="5\n");
}


static void testEvaluateEmpty()
{
  Diagram diagram;
  diagram.addNode("");
  ostringstream stream;
  evaluate(diagram,stream);
}


static void testEvaluateShowDisconnectedInput()
{
  Diagram diagram;
  diagram.addNode("show($)");
  ostringstream stream;
  evaluate(diagram,stream);
}


static void testEvaluateShowConnectedInput()
{
  Diagram diagram;
  int n1 = diagram.addNode("5");
  int n2 = diagram.addNode("show($)");
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
  int n1 = diagram.addNode("5");
  int n2 = diagram.addNode("show($)");
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
  int n1 = diagram.addNode("5");
  int n2 = diagram.addNode("b=$");
  diagram.connectNodes(n1,0,n2,0);
  diagram.deleteNode(n1);
  assert(diagram.findNode(n2));
  assert(diagram.node(n2).inputs[0].source_node_index==nullNodeIndex());
  ostringstream stream;
  diagram.removeInvalidInputs();
  evaluate(diagram,stream);
}


static void testExistingNodeIndices()
{
  Diagram diagram;
  assert(diagram.existingNodeIndices().size()==0);
}


static void testEvaluatingIncompleteVectorOverTwoLines()
{
  Diagram diagram;
  diagram.addNode("[1,2\n");

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
