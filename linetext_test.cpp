#include "linetext.hpp"

#include <cassert>
#include <sstream>
#include <string>


using std::ostringstream;
using std::string;


static float lineTextValue(const char *text,float input_value)
{
  ostringstream stream;
  float result = lineTextValue(text,stream,input_value);
  string output = stream.str();
  assert(output=="");
  return result;
}


int main()
{
  assert(lineTextHasInput("x=$"));
  assert(lineTextHasInput("cos($)"));
  assert(!lineTextHasOutput("let x=5"));
  assert(!lineTextHasOutput("x=$"));
  assert(!lineTextHasOutput(""));
  assert(!lineTextHasOutput(" "));
  assert(!lineTextHasOutput(")"));
  assert(lineTextValue("5")==5);
  assert(lineTextValue("t")==0);
  assert(lineTextValue("$",5)==5);
  assert(lineTextValue("")==0);
  assert(lineTextHasInput("  body=$,"));
}
