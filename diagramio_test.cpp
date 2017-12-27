#include "diagramio.hpp"

#include <cassert>
#include <sstream>


using std::istringstream;
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


static Diagram scanFromText(const string &text)
{
  Diagram diagram;
  istringstream stream(text);
  scanDiagramFrom(stream,diagram);
  return diagram;
}


static void testWithEmptyDiagram()
{
  Diagram d;
  string diagram_text = makeText(d);
  const char *expected_text =
    "diagram {\n"
    "}\n";
  assert(diagram_text==expected_text);
  Diagram d2 = scanFromText(diagram_text);
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
  Diagram d2 = scanFromText(diagram_text);
  assert(d2.nExistingNodes()==1);
  assert(d2.node(0).position()==Point2D(0,0));
  assert(d2.node(0).lines[0].text=="x=5");
}


static void testWithTwoConnectedNodes()
{
  Diagram d;
  NodeIndex n1 = d.addNode("x=$");
  NodeIndex n2 = d.addNode("5");
  d.node(n1).setPosition({101,102});
  d.connectNodes(n2,0,n1,0);
  const char *expected_text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position: [101,102]\n"
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
  string diagram_text = makeText(d);
  assert(diagram_text==expected_text);
  Diagram d2 = scanFromText(diagram_text);
  assert(d2.nExistingNodes()==2);
  assert(d2.node(0).position()==Point2D(101,102));
}


int main()
{
  testWithEmptyDiagram();
  testWithOneNode();
  testWithTwoConnectedNodes();
}
