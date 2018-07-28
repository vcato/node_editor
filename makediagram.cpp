#include "makediagram.hpp"

#include <sstream>
#include "diagramio.hpp"


Diagram makeDiagram(const char *text)
{
  std::istringstream stream(text);
  Diagram diagram;
  scanDiagramFrom(stream,diagram);
  return diagram;
}
