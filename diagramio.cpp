#include "diagramio.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include "streamparser.hpp"
#include "stringutil.hpp"
#include "wrapperstate.hpp"
#include "optional.hpp"
#include "diagramwrapperstate.hpp"

using std::string;
using std::ostream;


void printDiagramOn(ostream &stream,const Diagram &diagram)
{
  printStateOn(stream,makeDiagramWrapperState(diagram));
}


static bool scanDiagram(Diagram &diagram,StreamParser &parser)
{
  parser.scanWord();

  Optional<WrapperState> maybe_state = scanState(parser);

  if (!maybe_state) {
    return false;
  }

  return buildDiagramFromState(diagram,*maybe_state,parser.error);
}


void scanDiagramFrom(std::istream &stream,Diagram &diagram,string &error)
{
  StreamParser parser(stream);

  if (!scanDiagram(diagram,parser)) {
    std::ostringstream error_stream;
    error_stream << "error on line " << parser.line_number << ": " <<
      parser.error;
    error = error_stream.str();
  }
}
