#include "linetext.hpp"

#include <iostream>

using std::cerr;


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


static char peek(const std::string &text,int index)
{
  int text_length = text.length();

  if (index==text_length) {
    return '\0';
  }

  return text[index];
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
  if (c>='0' && c<='9') return true;
  return false;
}


static bool skipIdentifier(const std::string &text,int &index)
{
  if (!isBeginIdentifierChar(peek(text,index))) {
    return false;
  }

  ++index;

  while (isIdentifierChar(peek(text,index))) {
    ++index;
  }

  return true;
}


static bool isWhitespace(char c)
{
  if (c==' ') return true;
  return false;
}

static void skipWhitespace(const std::string &text,int &index)
{
  while (isWhitespace(peek(text,index))) {
    ++index;
  }
}


static bool isAssignment(const std::string &text)
{
  int index = 0;
  if (!skipIdentifier(text,index)) {
    return false;
  }
  skipWhitespace(text,index);
  if (peek(text,index)!='=') {
    return false;
  }
  return true;
}


bool lineTextHasOutput(const std::string &text)
{
  if (text=="") return false;
  if (isAssignment(text)) return false;

  return true;
}
