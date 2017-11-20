#include "linetext.hpp"

#include <cassert>


int main()
{
  assert(lineTextHasInput("x=$"));
  assert(lineTextHasInput("cos($)"));
  assert(!lineTextHasOutput("x=$"));
  assert(!lineTextHasOutput(""));
  assert(lineTextValue("5")==5);
  assert(lineTextValue("t")==0);
}
