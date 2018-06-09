#include "linetext.hpp"

#include <cassert>
#include <algorithm>
#include <sstream>
#include <functional>
#include "any.hpp"



using std::cerr;
using std::string;
using std::vector;


int lineTextInputCount(const string &text)
{
  return std::count(text.begin(),text.end(),'$');
}


static bool isBeginIdentifierChar(char c)
{
  if (c>='a' && c<='z') return true;
  if (c>='A' && c<='Z') return true;
  if (c=='_') return true;
  return false;
}


static bool isDigit(char c)
{
  if (c>='0' && c<='9') return true;
  return false;
}


static bool isIdentifierChar(char c)
{
  if (isBeginIdentifierChar(c)) return true;
  if (isDigit(c)) return true;
  return false;
}


static bool isWhitespace(char c)
{
  if (c==' ') return true;
  return false;
}


namespace {
struct Parser {
  const std::string &text;
  int &index;

  char peek() const
  {
    int text_length = text.length();

    if (index==text_length) {
      return '\0';
    }

    return text[index];
  }

  bool atEnd() const
  {
    return peek()=='\0';
  }

  bool skipIdentifier()
  {
    if (!isBeginIdentifierChar(peek())) {
      return false;
    }

    ++index;

    while (isIdentifierChar(peek())) {
      ++index;
    }

    return true;
  }

  bool skipChar()
  {
    assert(!atEnd());
    ++index;
    return true;
  }

  bool skipNumber()
  {
    if (!isDigit(peek())) return false;

    while (isDigit(peek())) {
      ++index;
    }

    return true;
  }

  void skipWhitespace()
  {
    while (isWhitespace(peek())) {
      ++index;
    }
  }

  bool getIdentifier(string &identifier)
  {
    if (!isIdentifierChar(peek())) {
      return false;
    }

    int start = index;
    skipIdentifier();
    int end = index;
    identifier = text.substr(start,end-start);
    return true;
  }

  bool getNumber(float &number)
  {
    skipWhitespace();

    int start = index;

    if (!skipNumber()) {
      return false;
    }

    int end = index;

    number = stoi(text.substr(start,end-start));
    return true;
  }
};
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

  if (parser.peek()!='=') {
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


namespace {
struct ValueFunction {
  virtual void operator()(const Any &) const = 0;
};
}

static bool
  evaluateExpression(
    Parser &parser,
    const ValueFunction &handler,
    const vector<float> &input_values,
    int &input_index
  )
{
  float value = 0;

  if (parser.getNumber(value)) {
    handler(value);
    return true;
  }

  if (parser.peek()=='$') {
    float value = input_values[input_index];
    ++input_index;
    ++parser.index;
    handler(value);
    return true;
  }

  if (parser.peek()=='[') {
    parser.skipChar();

    if (parser.peek()==']') {
      assert(false);
    }

    assert(false);
  }

  return false;
}


float
  evaluateLineText(
    const string &line_text_arg,
    const vector<float> &input_values,
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
      if (parser.peek()!='(') {
        return 0;
      }

      ++character_index;

      struct ShowFunction : ValueFunction {
        Executor &executor;

        ShowFunction(Executor &executor_arg)
        : executor(executor_arg)
        {
        }

        void operator()(const Any &arg) const
        {
          switch (arg.type) {
            case Any::float_type:
              executor.executeShow(arg.as<float>());
              break;
            case Any::void_type:
              assert(false);
              break;
          }
        }
      };

      bool was_evaluated =
        evaluateExpression(
          parser,
          ShowFunction{executor},
          input_values,
          input_index
        );

      if (!was_evaluated) {
        return 0;
      }

      if (parser.peek()!=')') {
        return 0;
      }

      ++character_index;

      if (!parser.atEnd()) {
        return 0;
      }

      return 0;
    }

    if (identifier=="return") {
      struct ReturnFunction : ValueFunction {
        Executor &executor;

        ReturnFunction(Executor &executor_arg)
        : executor(executor_arg)
        {
        }

        void operator()(const Any& arg) const
        {
          switch (arg.type) {
            case Any::void_type:
              assert(false);
              break;
            case Any::float_type:
              executor.executeReturn(arg.as<float>());
              break;
          }
        }
      };

      // bool was_evaluated =
        evaluateExpression(
          parser,
          ReturnFunction{executor},
          input_values,
          input_index
        );
      return 0;
    }

    return 0;
  }

  if (line_text=="$+$") {
    float input1 = input_values[input_index++];
    float input2 = input_values[input_index++];
    return input1 + input2;
  }

  if (line_text=="$") {
    auto value = input_values[input_index];
    ++input_index;
    return value;
  }

  return 0;
}
