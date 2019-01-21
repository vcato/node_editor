#include "optional.hpp"

#include <string>

using std::string;


static void testOptionalInt()
{
  Optional<int> v;

  assert(!v);
  assert(!v.hasValue());
  assert(!(v==5));

  v = 5;

  assert(v);
  assert(*v==5);
  assert(v==5);

  v.emplace();

  assert(v==0);

  v = Optional<int>(1);

  assert(v==1);

  v = Optional<int>();

  assert(!v);
}


static void testOptionalString()
{
  Optional<string> s("test");
  assert(*s=="test");
}


static void testEmplace()
{
  Optional<string> s;
  s.emplace();
  assert(s == string(""));
}


int main()
{
  testOptionalInt();
  testOptionalString();
  testEmplace();
}
