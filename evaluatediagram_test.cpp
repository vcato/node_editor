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


static Any
  evaluateDiagramReturningAny(
    const Diagram &diagram,
    const Environment &environment = {}
  )
{
  std::ostringstream show_stream;
  DiagramExecutionContext context{show_stream,/*error_stream*/cerr};
  context.parent_environment_ptr = &environment;
  DiagramExecutor executor(context, context.parent_environment_ptr);
  evaluateDiagram(diagram,executor);
  return std::move(*executor.maybe_return_value);
}


static Point2D
  evaluateDiagramReturningPoint2D(
    const Diagram &diagram,
    const Environment &environment = {}
  )
{
  Any return_value = evaluateDiagramReturningAny(diagram,environment);
  Optional<Point2D> maybe_point2d = maybePoint2D(return_value);
  assert(maybe_point2d);
  return *maybe_point2d;
}


static float
  evaluateDiagramReturningFloat(
    const Diagram &diagram,
    const Environment &environment = {}
  )
{
  Any return_value = evaluateDiagramReturningAny(diagram,environment);

  if (!return_value.isFloat()) {
    cerr << "Got a " << return_value.typeName() << " instead of float\n";
  }

  assert(return_value.isFloat());
  return return_value.asFloat();
}


static void testSimpleReturn()
{
  Diagram diagram;
  diagram.createNodeWithText("return [1,2]");
  Point2D result = evaluateDiagramReturningPoint2D(diagram);
  assert(result==Point2D(1,2));
}


static void testConnectedReturn()
{
  Diagram diagram;
  NodeIndex node1 = diagram.createNodeWithText("[1,2]");
  NodeIndex node2 = diagram.createNodeWithText("return $");
  diagram.connectNodes(node1,0,node2,0);
  Point2D result = evaluateDiagramReturningPoint2D(diagram);
  assert(result==Point2D(1,2));
}


static void testBuildingVector()
{
  Diagram diagram;
  NodeIndex x_node = diagram.createNodeWithText("x");
  NodeIndex y_node = diagram.createNodeWithText("y");
  NodeIndex build_vector_node = diagram.createNodeWithText("[$,$]");
  NodeIndex return_node = diagram.createNodeWithText("return $");
  diagram.connectNodes(x_node,0,build_vector_node,0);
  diagram.connectNodes(y_node,0,build_vector_node,1);
  diagram.connectNodes(build_vector_node,0,return_node,0);

  Environment environment;
  environment["x"] = 1;
  environment["y"] = 2;

  Point2D result = evaluateDiagramReturningPoint2D(diagram,environment);
  assert(result==Point2D(1,2));
}


static void testLocalPositionDiagram()
{
  Diagram diagram = localPositionDiagram();

  Environment environment;
  environment["x"] = 1;
  environment["y"] = 2;

  Point2D result = evaluateDiagramReturningPoint2D(diagram,environment);
  assert(result==Point2D(1,2));
}


static void testAveragingDiagram()
{
  Diagram diagram = makeDiagram(averaging_diagram_text);
  Environment environment;
  environment["x"] = 1;
  environment["y"] = 3;

  Point2D result = evaluateDiagramReturningPoint2D(diagram,environment);
  assert(result==Point2D(2,2));
}


static void testExpression()
{
  Diagram diagram;
  Environment environment;
  environment["x"] = 6;
  diagram.createNodeWithText("return x+5");
  float result = evaluateDiagramReturningFloat(diagram,environment);
  assert(result==11);
}


static void testDiagramReturningWrongType()
{
  string expected_error_message = "Returning float instead of vector.\n";
  Diagram diagram;
  NodeIndex node_index = diagram.createNodeWithText("return 5");
  string expected_return_type_name = Any(vector<Any>{1,2}).typeName();

  std::ostringstream show_stream;
  DiagramExecutionContext context{show_stream,/*error_stream*/cerr};
  DiagramExecutor executor(context, /*parent_environment_ptr*/nullptr);
  executor.optional_expected_return_type_name = expected_return_type_name;
  DiagramState diagram_state;
  evaluateDiagram(diagram,executor,diagram_state);

  assert(!executor.maybe_return_value);

  string error_message = diagram_state.node_states[node_index].line_errors[0];

  if (error_message != expected_error_message) {
    cerr << "error_message: " << error_message << "\n";
  }

  assert(error_message == expected_error_message);
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
}
