#ifndef PARSER_HPP_
#define PARSER_HPP_


#include <string>


class Parser {
  public:
    inline Parser(const std::string &text_arg,int &index_arg);
    inline char peek() const;
    inline bool atEnd() const;
    inline bool skipIdentifier() const;
    inline bool skipChar() const;
    inline bool skipNumber() const;
    inline void skipWhitespace() const;
    inline bool getIdentifier(std::string &identifier) const;
    inline bool getNumber(float &number) const;

  private:
    const std::string &text;
    int &index;

    static bool isWhitespace(char c)
    {
      if (c==' ') return true;
      return false;
    }

    static bool isDigit(char c)
    {
      if (c>='0' && c<='9') return true;
      return false;
    }

    static bool isBeginIdentifierChar(char c)
    {
      if (c>='a' && c<='z') return true;
      if (c>='A' && c<='Z') return true;
      if (c=='_') return true;
      return false;
    }

    static bool isIdentifierChar(char c)
    {
      if (isBeginIdentifierChar(c)) return true;
      if (isDigit(c)) return true;
      return false;
    }
};


Parser::Parser(const std::string &text_arg,int &index_arg)
: text(text_arg), index(index_arg)
{
}


char Parser::peek() const
{
  int text_length = text.length();

  if (index==text_length) {
    return '\0';
  }

  return text[index];
}


bool Parser::atEnd() const
{
  return peek()=='\0';
}


bool Parser::skipIdentifier() const
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


bool Parser::skipChar() const
{
  assert(!atEnd());
  ++index;
  return true;
}


bool Parser::skipNumber() const
{
  if (!isDigit(peek())) return false;

  while (isDigit(peek())) {
    ++index;
  }

  return true;
}


void Parser::skipWhitespace() const
{
  while (isWhitespace(peek())) {
    ++index;
  }
}


bool Parser::getIdentifier(std::string &identifier) const
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


bool Parser::getNumber(float &number) const
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


#endif /* PARSER_HPP_ */
