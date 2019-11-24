#include "diagramwrapperstate.hpp"

#include <iostream>
#include <sstream>
#include "diagram.hpp"
#include "wrapperstate.hpp"

using std::cerr;
using std::string;
using std::istringstream;
using std::ostringstream;


static WrapperState stateFromText(const char *test_text)
{
  istringstream stream(test_text);
  ScanStateResult scan_result = scanStateFrom(stream);
  return scan_result.asValue();
}


static const char *test_text =
  "diagram {\n"
  "  node {\n"
  "    id: 1\n"
  "    position {\n"
  "      x: 0\n"
  "      y: 0\n"
  "    }\n"
  "    line: \"return $\"\n"
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

static void testMakingStateWithEmptyDiagram()
{
  Diagram d;
  WrapperState s = makeDiagramWrapperState(d);
  assert(s.tag=="diagram");
  assert(s.children.empty());
}


static void testMakingStateWithNormalDiagram()
{
  const char *expected_text = test_text;

  Diagram d;
  NodeIndex n1 = d.createNodeWithText("return $");
  NodeIndex n2 = d.createNodeWithText("5");
  d.connectNodes(n2,0,n1,0);
  WrapperState s = makeDiagramWrapperState(d);

  ostringstream stream;
  printStateOn(stream,s);
  string text = stream.str();
  assert(text==expected_text);
}


static void testMakingDiagram()
{
  WrapperState state = stateFromText(test_text);
  Diagram diagram = makeDiagramFromWrapperState(state);
  assert(makeDiagramWrapperState(diagram)==state);
}


int main()
{
  testMakingStateWithEmptyDiagram();
  testMakingStateWithNormalDiagram();
  testMakingDiagram();
}
