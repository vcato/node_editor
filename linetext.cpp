#include "linetext.hpp"

#include <cassert>
#include <algorithm>
#include <sstream>
#include <functional>
#include "any.hpp"
#include "optional.hpp"
#include "parser.hpp"
#include "evaluateexpression.hpp"


using std::cerr;
using std::string;
using std::vector;


int lineTextInputCount(const string &text)
{
  return std::count(text.begin(),text.end(),'$');
}


static bool isAssignment(const std::string &text)
{
  int index = 0;

  Parser parser{text,index};

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


static bool isNumber(const std::string &text)
{
  int index = 0;
  Parser parser{text,index};

  parser.skipWhitespace();

  if (!parser.skipNumber()) {
    return false;
  }

  parser.skipWhitespace();

  if (!parser.atEnd()) return false;

  return true;
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


float
  evaluateLineText(
    const string &line_text_arg,
    const vector<Any> &input_values,
    Executor &executor
  )
{
  string line_text = trimmed(line_text_arg);
  int input_index = 0;

  if (isNumber(line_text)) {
    float result = std::stoi(line_text);
    return result;
  }

  int character_index = 0;
  Parser parser{line_text,character_index};
  string identifier;

  if (parser.getIdentifier(identifier)) {
    if (identifier=="show") {
      if (parser.peekChar()!='(') {
        return 0;
      }

      ++character_index;

      Optional<Any> maybe_value =
        evaluateExpression(
          parser,
          input_values,
          input_index
        );

      bool was_evaluated = maybe_value.hasValue();

      if (!was_evaluated) {
        return 0;
      }

      if (parser.peekChar()!=')') {
        return 0;
      }

      ++character_index;

      executor.executeShow(*maybe_value);

      if (!parser.atEnd()) {
        return 0;
      }

      return 0;
    }

    if (identifier=="return") {
      Optional<Any> maybe_value =
        evaluateExpression(
          parser,
          input_values,
          input_index
        );

      if (maybe_value) {
        executor.executeReturn(*maybe_value);
      }

      return 0;
    }

    return 0;
  }

  Optional<Any> maybe_result =
    evaluateExpression(parser,input_values,input_index);

  if (!maybe_result) {
    return 0;
  }

  const Any &result = *maybe_result;

  if (result.isFloat()) {
    return result.asFloat();
  }

  return 0;
}
