#include "linetext.hpp"

#include <cassert>
#include <algorithm>
#include <sstream>
#include <functional>
#include "any.hpp"
#include "optional.hpp"
#include "stringparser.hpp"
#include "evaluateexpression.hpp"
#include "rangetext.hpp"


using std::cerr;
using std::string;
using std::vector;
using std::ostream;


int lineTextInputCount(const string &text)
{
  return std::count(text.begin(),text.end(),'$');
}


static bool isAssignment(const std::string &text)
{
  int index = 0;

  StringParser parser{text,index};

  Optional<StringParser::Range> maybe_identifier_range =
    parser.maybeIdentifierRange();

  if (!maybe_identifier_range) {
    return false;
  }

  string identifier = rangeText(*maybe_identifier_range, parser.text);

  if (identifier=="let") {
    return true;
  }

  parser.skipWhitespace();

  if (parser.peekChar()!='=') {
    return false;
  }

  return true;
}


static bool isReturnStatement(const std::string &text)
{
  return startsWith(text,"return");
}


static string trimmed(const string &s)
{
  const char *whitespace = " ";
  string::size_type i = s.find_first_not_of(whitespace);
  if (i==string::npos) {
    return "";
  }
  return s.substr(i);
}


bool lineTextHasOutput(const std::string &text_arg)
{
  string text = trimmed(text_arg);
  if (text=="") return false;
  if (isAssignment(text)) return false;
  if (isReturnStatement(text)) return false;
  if (text==")") return false;

  return true;
}


