#ifndef STRINGPARSER_HPP_
#define STRINGPARSER_HPP_

#include <cassert>
#include <string>
#include "optional.hpp"
#include "stringrange.hpp"


class StringParser {
  public:
    using Range = StringRange;
    using Index = StringIndex;

    const std::string &text;

    inline StringParser(const std::string &text_arg,int &index_arg);
    inline char peekChar() const;
    inline bool atEnd() const;
    inline bool skipIdentifier() const;
    inline bool skipChar() const;
    inline bool skipNumber() const;
    inline void skipWhitespace() const;
    Index index() const { return _index; }
    Optional<Range> maybeNumberRange() const;
    Optional<Range> maybeIdentifierRange() const;
    std::string rangeText(const Range &range) const;

  private:
    struct Impl;

    int &_index;

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


StringParser::StringParser(const std::string &text_arg,int &index_arg)
: text(text_arg), _index(index_arg)
{
}


char StringParser::peekChar() const
{
  int text_length = text.length();

  if (_index==text_length) {
    return '\0';
  }

  return text[_index];
}


bool StringParser::atEnd() const
{
  return peekChar()=='\0';
}


bool StringParser::skipIdentifier() const
{
  if (!isBeginIdentifierChar(peekChar())) {
    return false;
  }

  ++_index;

  while (isIdentifierChar(peekChar())) {
    ++_index;
  }

  return true;
}


bool StringParser::skipChar() const
{
  assert(!atEnd());
  ++_index;
  return true;
}


bool StringParser::skipNumber() const
{
  if (!isDigit(peekChar())) return false;

  while (isDigit(peekChar())) {
    ++_index;
  }

  return true;
}


void StringParser::skipWhitespace() const
{
  while (isWhitespace(peekChar())) {
    ++_index;
  }
}


#endif /* STRINGPARSER_HPP_ */
