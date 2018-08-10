#include "any.hpp"


static void testObject()
{
  Any a{Object()};
  assert(a.isObject());
}


static void testClass()
{
  Any a{Class()};
  assert(a.isClass());
}


int main()
{
  testObject();
  testClass();
}
