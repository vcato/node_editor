#include "optionalpoint2d.hpp"

#include <sstream>
#include <iostream>
#include "diagramevaluation.hpp"
#include "diagramexecutor.hpp"


using std::vector;
using std::cerr;


static Point2D evaluateDiagramReturningPoint2D(const Diagram &diagram)
{
  std::ostringstream show_stream;
  DiagramExecutor executor(show_stream);
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


int main()
{
  testSimpleReturn();
}
