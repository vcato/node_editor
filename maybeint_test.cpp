#include "maybeint.hpp"


int main()
{
  assert(maybeInt("0")==0);
  assert(maybeInt("1")==1);
  assert(!maybeInt(""));
  assert(!maybeInt("x"));
  assert(!maybeInt("99999999999999999"));
}
