#include "makediagram.hpp"

#include <sstream>
#include <cassert>
#include "diagramio.hpp"

using std::string;


Diagram makeDiagram(const string &text)
{
  std::istringstream stream(text);
  Diagram diagram;
  string error;
  scanDiagramFrom(stream,diagram,error);
  assert(error.empty());
  return diagram;
}
