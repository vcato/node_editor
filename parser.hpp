#ifndef PARSER_HPP_
#define PARSER_HPP_


#include <string>


namespace parser {
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

  bool getIdentifier(std::string &identifier)
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


using parser::Parser;


#endif /* PARSER_HPP_ */
