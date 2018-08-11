#include "any.hpp"


using std::string;


static void testObject()
{
  struct Data : Object::Data {
    virtual Data *clone()
    {
      return new Data(*this);
    }
  };

  auto make_test_object_function = [&](const Class &test_class){
    return Object(&test_class,*new Data);
  };

  Class test_class(make_test_object_function);

  std::function<Any(const string &member_name)> make_member_function;
  Any a{ Object(&test_class,*new Data) };
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


static void testMemoryLeak1()
{
  std::vector<Any> v;
  v.push_back(5);
  Optional<Any> op{Any(std::move(v))};
  Optional<Any> op2{std::move(op)};
}


int main()
{
  testObject();
  testClass();
  testMemoryLeak1();
}
