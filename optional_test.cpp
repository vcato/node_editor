#include "optional.hpp"

#include <string>


static void testOptionalInt()
{
  Optional<int> v;
  assert(!v);
  v = 5;
  assert(v);
  assert(*v==5);
}


static void testOptionalString()
{
  Optional<std::string> s("test");
  assert(*s=="test");
}


int main()
{
  testOptionalInt();
  testOptionalString();
}
