#include "makediagram.hpp"

#include <sstream>
#include <cassert>
#include <iostream>
#include "diagramio.hpp"

using std::string;
using std::cerr;


Diagram makeDiagram(const string &text)
{
  std::istringstream stream(text);
  Diagram diagram;
  string error;
  scanDiagramFrom(stream,diagram,error);

  if (!error.empty()) {
    cerr << "error: " << error << "\n";
  }

  assert(error.empty());
  return diagram;
}
