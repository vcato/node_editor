#include "optionalpoint2d.hpp"

#include <sstream>
#include <iostream>
#include "diagramevaluation.hpp"
#include "diagramexecutor.hpp"
#include "defaultdiagrams.hpp"
#include "makediagram.hpp"


using std::vector;
using std::cerr;


static const char *averaging_diagram_text =
  "diagram {\n"
  "  node {\n"
  "    id: 1\n"
  "    position: [275,198]\n"
  "    text {\n"
  "      \"[$,$]\"\n"
  "    }\n"
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
  "    position: [20,187]\n"
  "    text {\n"
  "      \"x\"\n"
  "    }\n"
  "  }\n"
  "  node {\n"
  "    id: 3\n"
  "    position: [20,135]\n"
  "    text {\n"
  "      \"y\"\n"
  "    }\n"
  "  }\n"
  "  node {\n"
  "    id: 4\n"
  "    position: [436,183]\n"
  "    text {\n"
  "      \"return $\"\n"
  "    }\n"
  "    connection {\n"
  "      input_index: 0\n"
  "      source_node_id: 1\n"
  "      source_output_index: 0\n"
  "    }\n"
  "  }\n"
  "  node {\n"
  "    id: 5\n"
  "    position: [113,186]\n"
  "    text {\n"
  "      \"($+$)/2\"\n"
  "    }\n"
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


static Point2D
  evaluateDiagramReturningPoint2D(
    const Diagram &diagram,
    const DiagramExecutor::Environment &environment = {}
  )
{
  std::ostringstream show_stream;
  DiagramExecutor executor(show_stream);
  executor.environment = environment;
  evaluateDiagram(diagram,executor);
  assert(executor.return_value.isVector());
  const vector<Any> &return_vector = executor.return_value.asVector();
  assert(return_vector.size()==2);
  const Any &any_x = return_vector[0];
  const Any &any_y = return_vector[1];
  assert(any_x.isFloat());
  assert(any_y.isFloat());
  return Point2D(any_x.asFloat(),any_y.asFloat());
}


static void testSimpleReturn()
{
  Diagram diagram;
  diagram.addNode("return [1,2]");
  Point2D result = evaluateDiagramReturningPoint2D(diagram);
  assert(result==Point2D(1,2));
}


static void testConnectedReturn()
{
  Diagram diagram;
  NodeIndex node1 = diagram.addNode("[1,2]");
  NodeIndex node2 = diagram.addNode("return $");
  diagram.connectNodes(node1,0,node2,0);
  Point2D result = evaluateDiagramReturningPoint2D(diagram);
  assert(result==Point2D(1,2));
}


static void testBuildingVector()
{
  Diagram diagram;
  NodeIndex x_node = diagram.addNode("x");
  NodeIndex y_node = diagram.addNode("y");
  NodeIndex build_vector_node = diagram.addNode("[$,$]");
  NodeIndex return_node = diagram.addNode("return $");
  diagram.connectNodes(x_node,0,build_vector_node,0);
  diagram.connectNodes(y_node,0,build_vector_node,1);
  diagram.connectNodes(build_vector_node,0,return_node,0);

  DiagramExecutor::Environment environment;
  environment["x"] = 1;
  environment["y"] = 2;

  Point2D result = evaluateDiagramReturningPoint2D(diagram,environment);
  assert(result==Point2D(1,2));
}


static void testLocalPositionDiagram()
{
  Diagram diagram = localPositionDiagram();

  DiagramExecutor::Environment environment;
  environment["x"] = 1;
  environment["y"] = 2;

  Point2D result = evaluateDiagramReturningPoint2D(diagram,environment);
  assert(result==Point2D(1,2));
}


static void testAveragingDiagram()
{
  Diagram diagram = makeDiagram(averaging_diagram_text);
  DiagramExecutor::Environment environment;
  environment["x"] = 1;
  environment["y"] = 3;

  Point2D result = evaluateDiagramReturningPoint2D(diagram,environment);
  assert(result==Point2D(2,2));
}


int main()
{
  testSimpleReturn();
  testConnectedReturn();
  testBuildingVector();
  testLocalPositionDiagram();
  testAveragingDiagram();
}
