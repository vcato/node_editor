#include "any.hpp"


using std::string;


static void testObject()
{
  auto test_member_function =
    [](const std::string &/*member_name*/) -> Optional<Any> { return {}; };

  auto make_test_object_function = [&](const Class &test_class){
    return Object(&test_class,test_member_function);
  };

  Class test_class(make_test_object_function);

  std::function<Any(const string &member_name)> make_member_function;
  Any a{ Object(&test_class,make_member_function) };
  assert(a.isObject());
  assert(a.asObject()==a.asObject());
}


static void testClass()
{
  std::function<Object(const Class &)> make_object_function;
  Class c(make_object_function);
  Any a(&c);
  assert(a.isClassPtr());
  assert(a.asClassPtr() == &c);
}


int main()
{
  testObject();
  testClass();
}
