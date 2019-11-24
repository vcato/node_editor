#ifndef WRAPPERSTATE_HPP
#define WRAPPERSTATE_HPP


#include <cassert>
#include <vector>
#include "basicvariant.hpp"
#include "createobject.hpp"
#include "wrappervalue.hpp"
#include "optional.hpp"
#include "streamparser.hpp"
#include "taggedvalue.hpp"
#include "taggedvalueio.hpp"


using ScanStateResult = ScanTaggedValueResult;
using WrapperState = TaggedValue;


extern void
  printStateOn(std::ostream &stream,const WrapperState &state,int indent = 0);

extern ScanStateResult scanStateFrom(std::istream &stream);

extern Optional<WrapperState> scanState(StreamParser &parser);
  // diagramio uses this as part of scanning a diagram.

//extern void scanChildrenSection(WrapperState &state,StreamParser &parser);

#endif /* WRAPPERSTATE_HPP */
