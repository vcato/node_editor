#include "wrapperstate.hpp"

#include "taggedvalueio.hpp"

using std::ostream;


void printStateOn(ostream &stream,const WrapperState &state,int indent)
{
  printTaggedValueOn(stream,state,indent);
}


Optional<WrapperState> scanState(StreamParser &parser)
{
  return scanTaggedValue(parser);
}


ScanStateResult scanStateFrom(std::istream & stream)
{
  return scanTaggedValueFrom(stream);
}
