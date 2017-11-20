#include "linetext.hpp"

#include <cassert>


int main()
{
  assert(lineTextHasInput("x=$"));
  assert(lineTextHasInput("cos($)"));
  assert(!lineTextHasOutput("x=$"));
  assert(!lineTextHasOutput(""));
}
