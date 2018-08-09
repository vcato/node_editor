#include "linetext.hpp"

#include <cassert>
#include <algorithm>
#include <sstream>
#include <functional>
#include "any.hpp"
#include "optional.hpp"
#include "stringparser.hpp"
#include "evaluateexpression.hpp"


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

  {
    string identifier;

    if (!parser.getIdentifier(identifier)) {
      return false;
    }

    if (identifier=="let") {
      return true;
    }
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


Optional<Any>
  evaluateLineText(
    const string &line_text_arg,
    const vector<Any> &input_values,
    Executor &executor,
    ostream &error_stream
  )
{
  int character_index = 0;
  StringParser parser{line_text_arg,character_index};
  parser.skipWhitespace();
  int input_index = 0;

  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    executor.environment
  };

  string identifier;

  if (parser.getIdentifier(identifier)) {
    if (identifier=="show") {
      if (parser.peekChar()!='(') {
        return {};
      }

      parser.skipChar();

      Optional<Any> maybe_value = evaluateExpression(data);

      if (!maybe_value) {
        return {};
      }

      if (parser.peekChar()!=')') {
        return {};
      }

      parser.skipChar();

      executor.executeShow(*maybe_value);

      if (!parser.atEnd()) {
        return {};
      }

      return Any();
    }

    if (identifier=="return") {
      Optional<Any> maybe_value = evaluateExpression(data);

      if (!maybe_value) {
        return {};
      }

      executor.executeReturn(*maybe_value);

      return Any();
    }

    return evaluateExpressionStartingWithIdentifier(data,identifier);
  }

  return evaluateExpression(data);
}
