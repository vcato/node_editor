#include "linetext.hpp"

using std::string;


static bool lineTextHasInput(const string &text)
{
  return lineTextInputCount(text)>0;
}


int main()
{
  assert(lineTextHasInput("x=$"));
  assert(lineTextHasInput("$.pos("));
  assert(lineTextHasInput("cos($)"));
  assert(lineTextHasInput("  body=$,"));
  assert(!lineTextHasOutput("let x=5"));
  assert(!lineTextHasOutput("x=$"));
  assert(!lineTextHasOutput(""));
  assert(!lineTextHasOutput(" "));
  assert(!lineTextHasOutput(")"));
  assert(!lineTextHasOutput("return $"));
}
