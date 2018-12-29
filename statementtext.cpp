#include "statementtext.hpp"

#include <vector>
#include <iostream>
#include <cassert>
#include <cstring>


using std::vector;
using std::string;
using std::cerr;


namespace {
struct Parser {
  Parser(const char *text_arg)
  : line_index(0),
    p(text_arg)
  {
    assert(p[0]=='\0' || p[strlen(p)-1]=='\n');
  }

  int lineIndex() const { return line_index; }
  char nextChar() const
  {
    assert(p);
    return *p;
  }

  void skipChar()
  {
    assert(p);
    assert(*p!='\0');
    if (*p=='\n') {
      ++line_index;
    }
    ++p;
  }

  bool atEnd() const
  {
    return nextChar()=='\0';
  }

  private:
    int line_index;
    const char *p;
};
}


static void skipGroup(Parser &parser);


static void skipToMatch(Parser &parser,char match)
{
  parser.skipChar();
  while (!parser.atEnd() && parser.nextChar()!=match) {
    skipGroup(parser);
  }
  if (!parser.atEnd()) {
    assert(parser.nextChar()==match);
    parser.skipChar();
  }
}


static void skipGroup(Parser &parser)
{
  if (parser.nextChar()=='[') {
    skipToMatch(parser,']');
  }
  else if (parser.nextChar()=='(') {
    skipToMatch(parser,')');
  }
  else {
    parser.skipChar();
  }
}


vector<int> statementLineCounts(const string &text)
{
  vector<int> result;
  if (text.empty()) return result;

  Parser parser(text.c_str());
  while (!parser.atEnd()) {
    int first_line_index = parser.lineIndex();
    for (;;) {
      if (parser.atEnd()) {
        break;
      }
      if (parser.nextChar()=='\n') {
        parser.skipChar();
        break;
      }
      skipGroup(parser);
    }
    int last_line_index = parser.lineIndex();
    result.push_back(last_line_index-first_line_index);
  }
  return result;
}
