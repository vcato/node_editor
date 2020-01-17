#include "stringparser.hpp"

using std::string;


struct StringParser::Impl {
};


string StringParser::rangeText(const Range &range) const
{
  return text.substr(range.begin, range.end - range.begin);
}


auto StringParser::maybeNumberRange() const -> Optional<Range>
{
  const StringParser &self = *this;
  self.skipWhitespace();

  Index start = self._index;

  if (!self.skipNumber()) {
    return {};
  }

  Index end = self._index;

  return Range{start, end};
}
