#include "streamparser.hpp"

#include <sstream>
#include <cassert>
#include <iostream>


using std::cerr;



static void testCase1()
{
  const char *text =
   "text {\n"
   "  \"x=5\"\n"
   "}\n";

  std::istringstream stream(text);
  StreamParser parser(stream);

  parser.scanWord();
  assert(parser.word=="text");
  assert(parser.line_number==1);

  parser.scanWord();
  assert(parser.word=="{");
  assert(parser.line_number==1);

  parser.scanEndOfLine();

  parser.scanWord();
  assert(parser.word=="\"x=5\"");
  assert(parser.line_number==2);

  parser.scanEndOfLine();

  parser.scanWord();
  assert(parser.word=="}");
  assert(parser.line_number==3);
}


static void testWithBlankLine()
{
  const char *text =
   "\n"
   "text\n";

  std::istringstream stream(text);
  StreamParser parser(stream);
  parser.scanWord();
  assert(parser.word=="text");
  assert(parser.line_number==2);
}


int main()
{
  testCase1();
  testWithBlankLine();
}
