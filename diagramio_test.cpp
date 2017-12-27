#include "diagramio.hpp"

#include <cassert>
#include <sstream>


using std::ostringstream;
using std::string;
using std::cerr;


static string makeText(const Diagram &d)
{
  ostringstream stream;
  printDiagramOn(stream,d);
  string diagram_text = stream.str();
  return diagram_text;
}


static void testWithEmptyDiagram()
{
  Diagram d;
  string diagram_text = makeText(d);
  const char *expected_text =
    "diagram {\n"
    "}\n";
  assert(diagram_text==expected_text);
}


static void testWithOneNode()
{
  Diagram d;
  d.addNode("x=5");
  const char *expected_text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position: [0,0]\n"
    "    text {\n"
    "      \"x=5\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  string diagram_text = makeText(d);
  assert(diagram_text==expected_text);
}


static void testWithTwoConnectedNodes()
{
  Diagram d;
  NodeIndex n1 = d.addNode("x=$");
  NodeIndex n2 = d.addNode("5");
  d.connectNodes(n2,0,n1,0);
  const char *expected_text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position: [0,0]\n"
    "    text {\n"
    "      \"x=$\"\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 2\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 2\n"
    "    position: [0,0]\n"
    "    text {\n"
    "      \"5\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  assert(makeText(d)==expected_text);
}


int main()
{
  testWithEmptyDiagram();
  testWithOneNode();
  testWithTwoConnectedNodes();
}
