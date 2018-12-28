#include "diagramio.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include "makediagram.hpp"


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


static void testWithEmptyDiagram()
{
  Diagram d;
  string diagram_text = makeText(d);
  const char *expected_text =
    "diagram {\n"
    "}\n";
  assert(diagram_text==expected_text);
  Diagram d2 = makeDiagram(diagram_text);
}


static void testWithOneNodeAndUnknownSection()
{
  const char *diagram_text =
    "diagram {\n"
    "  blah {\n"
    "    blah {\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 1\n"
    "    position: [0,0]\n"
    "    text {\n"
    "      \"x=5\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  Diagram d = makeDiagram(diagram_text);
  assert(d.nExistingNodes()==1);
  assert(d.node(0).position()==Point2D(0,0));
  assert(d.node(0).lines[0].text=="x=5");
}


static void testWithOneNode()
{
  Diagram d;
  d.addNode("x=5");
  const char *expected_text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position {\n"
    "      x: 0\n"
    "      y: 0\n"
    "    }\n"
    "    line: \"x=5\"\n"
    "  }\n"
    "}\n";
  string diagram_text = makeText(d);
  assert(diagram_text==expected_text);
  Diagram d2 = makeDiagram(diagram_text);
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
    "    position {\n"
    "      x: 101\n"
    "      y: 102\n"
    "    }\n"
    "    line: \"x=$\"\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 2\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 2\n"
    "    position {\n"
    "      x: 0\n"
    "      y: 0\n"
    "    }\n"
    "    line: \"5\"\n"
    "  }\n"
    "}\n";

  string diagram_text = makeText(d);
  assert(diagram_text==expected_text);
  Diagram d2 = makeDiagram(diagram_text);
  assert(d2.nExistingNodes()==2);
  assert(d2.node(0).position()==Point2D(101,102));
}


static void testWithTwoConnections()
{
  Diagram d;
  NodeIndex n1 = d.addNode("1");
  NodeIndex n2 = d.addNode("2");
  NodeIndex n3 = d.addNode("$+$");
  d.connectNodes(n1,0,n3,0);
  d.connectNodes(n2,0,n3,1);
  Diagram d2 = makeDiagram(makeText(d));
  assert(d2.node(n1).outputs.size()==1);
}


static void testWithMultiLineText()
{
  Diagram d;
  d.addNode("5\n6");
  Diagram d2 = makeDiagram(makeText(d));
}


static void testBadText(const string &text,const string &expected_error)
{
  istringstream stream(text);
  string error;
  Diagram diagram;
  scanDiagramFrom(stream,diagram,error);

  if (error!=expected_error) {
    cerr << "text: " << text << "\n";
    cerr << "error: " << error << "\n";
    cerr << "expected_error: " << expected_error << "\n";
  }

  assert(error==expected_error);
}


static void testWithBadText()
{
  {
    const char *text =
      "diagram{\n"
      "}\n";
    testBadText(text,"error on line 1: Expected tag 'diagram'");
  }
  {
    const char *text =
      "diagram\n"
      "}\n";
    testBadText(text,"error on line 1: Expected '{'");
  }
  {
    const char *text =
      "diagram {\n"
      "  blah\n"
      "}\n";
    testBadText(text,"error on line 2: Expected '{'");
  }
}


static void testWithUnknownSection()
{
  {
    const char *text =
      "diagram {\n"
      "  blah {\n"
      "  }\n"
      "}\n";
    makeDiagram(text);
  }
  {
    const char *text =
      "diagram {\n"
      "  blah {\n"
      "    blah {\n"
      "    }\n"
      "  }\n"
      "}\n";
    makeDiagram(text);
  }
  {
    const char *text =
      "diagram {\n"
      "  blah: 5\n"
      "}\n";
    makeDiagram(text);
  }
  {
    const char *text =
      "diagram {\n"
      "  blah {\n"
      "    blah: 5\n"
      "  }\n"
      "}\n";
    makeDiagram(text);
  }
  {
    const char *text =
      "diagram {\n"
      "  blah {\n"
      "    blah {\n"
      "      blah {\n"
      "      }\n"
      "    }\n"
      "  }\n"
      "}\n";
    makeDiagram(text);
  }
}


int main()
{
  testWithEmptyDiagram();
  testWithOneNode();
  testWithOneNodeAndUnknownSection();
  testWithTwoConnectedNodes();
  testWithTwoConnections();
  testWithMultiLineText();
  testWithBadText();
  testWithUnknownSection();
}
