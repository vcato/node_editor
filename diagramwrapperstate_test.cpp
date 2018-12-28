#include "diagram.hpp"
#include "wrapperstate.hpp"


#define ADD_TEST 0


static WrapperState makeDiagramWrapperState(const Diagram &/*diagram*/)
{
  WrapperState result("diagram");
  return result;
}


static void testEmptyDiagram()
{
  Diagram d;
  WrapperState s = makeDiagramWrapperState(d);
  assert(s.tag=="diagram");
  assert(s.children.empty());
}


#if ADD_TEST
static void testNormalDiagram()
{
  Diagram d;
  NodeIndex n1 = d.addNode("return $");
  NodeIndex n2 = d.addNode("5");
  d.connectNodes(n2,0,n1,0);
  WrapperState s = makeDiagramWrapperState(d);

  // Check to see that the wrapper state has two nodes with the appropriate
  // text and the nodes are connected.
  assert(false);
}
#endif


int main()
{
  testEmptyDiagram();
#if ADD_TEST
  testNormalDiagram();
#endif
}
