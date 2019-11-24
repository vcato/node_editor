#include "wrapperstate.hpp"

#include <algorithm>
#include <sstream>
#include "optional.hpp"
#include "streamparser.hpp"
#include "stringutil.hpp"
#include "printindent.hpp"
#include "taggedvalueio.hpp"

using std::istringstream;
using std::string;
using std::ostream;
using std::cerr;


void printStateOn(ostream &stream,const WrapperState &state,int indent)
{
  printTaggedValueOn(stream,state,indent);
}


Optional<WrapperState> scanState(StreamParser &parser)
{
  return scanTaggedValue(parser);
}


static ScanTaggedValueResult error(const std::string &message)
{
  return ScanTaggedValueResult::Error{message};
}


ScanTaggedValueResult scanTaggedValueFrom(std::istream & stream)
{
  StreamParser parser(stream);

  parser.scanWord();

  if (!parser.stream) {
    assert(false); // not implemented
  }

  Optional<WrapperState> maybe_state = scanState(parser);

  if (!maybe_state) {
    return error(parser.error);
  }

  return *maybe_state;
}


ScanStateResult scanStateFrom(std::istream & stream)
{
  return scanTaggedValueFrom(stream);
}
