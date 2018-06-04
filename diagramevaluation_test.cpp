#include "optionalpoint2d.hpp"


#include "diagramevaluation.hpp"


static OptionalPoint2D evaluateDiagramReturningPoint2D(const Diagram &)
{
  return {};
}


int main()
{
  Diagram diagram;
  diagram.addNode("return [1,2]");
  OptionalPoint2D maybe_point2d = evaluateDiagramReturningPoint2D(diagram);
  // assert(maybe_point2d);
}
