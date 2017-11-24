#include "linetext.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

using std::cerr;
using std::string;
using std::ostringstream;
using std::ostream;


bool lineTextHasInput(const std::string &text)
{
  if (endsWith(text,"$")) {
    return true;
  }

  if (contains(text,"$)")) {
    return true;
  }

  return false;
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


static bool isNumber(const std::string &text)
{
  int index = 0;
  Parser parser{text,index};

  parser.skipWhitespace();

#if 0
  if (!isDigit(parser.peek())) return false;

  while (isDigit(parser.peek())) {
    ++index;
  }
#else
  if (!parser.skipNumber()) {
    return false;
  }
#endif

  parser.skipWhitespace();

  if (!parser.atEnd()) return false;

  return true;
}


bool lineTextHasOutput(const std::string &text)
{
  if (text=="") return false;
  if (isAssignment(text)) return false;

  return true;
}


float lineTextValue(const string &line_text)
{
  ostringstream dummy_stream;
  float input_value = 0;
  return lineTextValue(line_text,dummy_stream,input_value);
}


float
  lineTextValue(
    const string &line_text,
    ostream &stream,
    const float input_value
  )
{
  if (isNumber(line_text)) {
    return std::stoi(line_text);
  }

  int index = 0;
  Parser parser{line_text,index};
  string identifier;
  if (parser.getIdentifier(identifier)) {
    if (identifier=="show") {
      if (parser.peek()!='(') {
        return 0;
      }
      ++index;
      float value = 0;
      if (parser.getNumber(value)) {
        stream << value << "\n";
      }
      if (parser.peek()=='$') {
        stream << input_value << "\n";
        ++index;
      }
      if (parser.peek()!=')') {
        return 0;
      }
      ++index;
      if (!parser.atEnd()) {
        return 0;
      }
      return 0;
    }
    return 0;
  }

  if (line_text=="$") {
    return input_value;
  }

  return 0;
}
