#include "optionalpoint2d.hpp"

#include <sstream>
#include <iostream>
#include "evaluatediagram.hpp"
#include "diagramexecutor.hpp"
#include "defaultdiagrams.hpp"
#include "makediagram.hpp"
#include "maybepoint2d.hpp"
#include "diagramio.hpp"

using std::vector;
using std::cerr;
using std::string;
using std::ostringstream;


static const char *averaging_diagram_text =
  "diagram {\n"
  "  node {\n"
  "    id: 1\n"
  "    position {\n"
  "      x: 275\n"
  "      y: 198\n"
  "    }\n"
  "    line: \"[$,$]\"\n"
  "    connection {\n"
  "      input_index: 0\n"
  "      source_node_id: 5\n"
  "      source_output_index: 0\n"
  "    }\n"
  "    connection {\n"
  "      input_index: 1\n"
  "      source_node_id: 5\n"
  "      source_output_index: 0\n"
  "    }\n"
  "  }\n"
  "  node {\n"
  "    id: 2\n"
  "    position {\n"
  "      x: 20\n"
  "      y: 187\n"
  "    }\n"
  "    line: \"x\"\n"
  "  }\n"
  "  node {\n"
  "    id: 3\n"
  "    position {\n"
  "      x: 20\n"
  "      y: 135\n"
  "    }\n"
  "    line: \"y\"\n"
  "  }\n"
  "  node {\n"
  "    id: 4\n"
  "    position {\n"
  "      x: 436\n"
  "      y: 183\n"
  "    }\n"
  "    line: \"return $\"\n"
  "    connection {\n"
  "      input_index: 0\n"
  "      source_node_id: 1\n"
  "      source_output_index: 0\n"
  "    }\n"
  "  }\n"
  "  node {\n"
  "    id: 5\n"
  "    position {\n"
  "      x: 113\n"
  "      y: 186\n"
  "    }\n"
  "    line: \"($+$)/2\"\n"
  "    connection {\n"
  "      input_index: 0\n"
  "      source_node_id: 2\n"
  "      source_output_index: 0\n"
  "    }\n"
  "    connection {\n"
  "      input_index: 1\n"
  "      source_node_id: 3\n"
  "      source_output_index: 0\n"
  "    }\n"
  "  }\n"
  "}\n";


namespace {
struct Tester {
  Diagram diagram;
  ostringstream show_stream;
  Environment environment;
  DiagramEvaluationState diagram_state;
  ostringstream error_stream;
  DiagramExecutionContext context{show_stream,error_stream,&environment};
  DiagramExecutor executor{context, context.parent_environment_ptr};

  void doEvaluation()
  {
    evaluateDiagram(diagram, executor, diagram_state);
  }

  Optional<Any> &maybeReturnValue()
  {
    return executor.maybe_return_value;
  }
};
}


namespace {
struct SingleNodeTester : Tester {
  const NodeIndex node_index;

  SingleNodeTester(const string &node_text)
  : node_index(diagram.createNodeWithText(node_text))
  {
  }

  vector<string> &lineErrors()
  {
    return diagram_state.node_states[node_index].line_errors;
  }

  vector<Any> &outputValues()
  {
    return diagram_state.node_states[node_index].output_values;
  }
};
}


static Point2D evaluateDiagramReturningPoint2D(Tester &tester)
{
  tester.doEvaluation();
  return *maybePoint2D(*tester.maybeReturnValue());
}


static float evaluateDiagramReturningFloat(Tester &tester)
{
  tester.doEvaluation();
  return tester.maybeReturnValue()->asFloat();
}


static void testSimpleReturn()
{
  Tester tester;
  tester.diagram.createNodeWithText("return [1,2]");
  Point2D result = evaluateDiagramReturningPoint2D(tester);
  assert(result == Point2D(1,2));
}


static void testConnectedReturn()
{
  Tester tester;
  Diagram &diagram = tester.diagram;
  NodeIndex node1 = diagram.createNodeWithText("[1,2]");
  NodeIndex node2 = diagram.createNodeWithText("return $");
  diagram.connectNodes(node1,0,node2,0);
  Point2D result = evaluateDiagramReturningPoint2D(tester);
  assert(result == Point2D(1,2));
}


static void testBuildingVector()
{
  Tester tester;
  Diagram &diagram = tester.diagram;
  NodeIndex x_node = diagram.createNodeWithText("x");
  NodeIndex y_node = diagram.createNodeWithText("y");
  NodeIndex build_vector_node = diagram.createNodeWithText("[$,$]");
  NodeIndex return_node = diagram.createNodeWithText("return $");
  diagram.connectNodes(x_node,0,build_vector_node,0);
  diagram.connectNodes(y_node,0,build_vector_node,1);
  diagram.connectNodes(build_vector_node,0,return_node,0);

  tester.environment["x"] = 1;
  tester.environment["y"] = 2;

  Point2D result = evaluateDiagramReturningPoint2D(tester);
  assert(result==Point2D(1,2));
}


static void testLocalPositionDiagram()
{
  Tester tester;
  tester.diagram = localPositionDiagram();
  tester.environment["x"] = 1;
  tester.environment["y"] = 2;

  Point2D result = evaluateDiagramReturningPoint2D(tester);

  assert(result==Point2D(1,2));
}


static void testAveragingDiagram()
{
  Tester tester;
  tester.diagram = makeDiagram(averaging_diagram_text);
  tester.environment["x"] = 1;
  tester.environment["y"] = 3;

  Point2D result = evaluateDiagramReturningPoint2D(tester);

  assert(result==Point2D(2,2));
}


static void testExpression()
{
  Tester tester;
  tester.environment["x"] = 6;
  tester.diagram.createNodeWithText("return x+5");

  float result = evaluateDiagramReturningFloat(tester);

  assert(result==11);
}


static void testDiagramReturningWrongType()
{
  string node_text = "return 5";
  string expected_return_type_name = Any(vector<Any>{1,2}).typeName();
  string expected_error_message = "Returning float instead of vector.\n";

  SingleNodeTester tester(node_text);

  DiagramExecutor &executor = tester.executor;
  executor.optional_expected_return_type_name = expected_return_type_name;
  tester.doEvaluation();

  assert(!tester.maybeReturnValue());
  assert(tester.lineErrors()[0] == expected_error_message);
}


static void testLocalVariable()
{
  string node_text = "x=5\nreturn x*x";
  float expected_return_value = 25;

  SingleNodeTester tester(node_text);

  tester.doEvaluation();

  assert(tester.maybeReturnValue() == expected_return_value);
}


static void testLocalVariableWithError()
{
  string node_text = "x=(5\nreturn x*x";
  string expected_line1_error_message = "Missing ')'\n";
  SingleNodeTester tester(node_text);

  tester.doEvaluation();

  assert(tester.lineErrors()[0] == expected_line1_error_message);
  assert(!tester.maybeReturnValue());
}


static void testStatementsCoveringMultipleLines()
{
  string node_text = "[1,\n2]";
  SingleNodeTester tester(node_text);
  tester.doEvaluation();
  Any expected_result = makeVector(1,2);
  assert(tester.outputValues()[0] == expected_result);
}


int main()
{
  testSimpleReturn();
  testConnectedReturn();
  testBuildingVector();
  testLocalPositionDiagram();
  testAveragingDiagram();
  testExpression();
  testDiagramReturningWrongType();
  testLocalVariable();
  testLocalVariableWithError();
  testStatementsCoveringMultipleLines();
}
