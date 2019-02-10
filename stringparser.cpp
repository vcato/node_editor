#include "stringparser.hpp"

#include "maybeint.hpp"


bool StringParser::getNumber(float &number) const
{
  skipWhitespace();

  int start = _index;

  if (!skipNumber()) {
    return false;
  }

  int end = _index;

  // This could throw an exception.  We should probably catch it and
  // return false.
  Optional<int> maybe_number = maybeInt(text.substr(start,end-start));

  if (!maybe_number) {
    assert(false); // not tested
  }

  number = *maybe_number;

  return true;
}
