#include "any.hpp"


using std::string;


static void testObject()
{
  struct Data : Object::Data {
    Data *clone() override
    {
      return new Data(*this);
    }

    Optional<Any> member(const std::string &/*member_name*/) override
    {
      assert(false);
    }
  };

  auto make_test_object_function = [&](const Class::NamedParameters &){
    return Object(*new Data);
  };

  Class test_class(make_test_object_function);

  std::function<Any(const string &member_name)> make_member_function;
  Any a{ Object(*new Data) };
  assert(a.isObject());
  assert(a.asObject()==a.asObject());
}


static void testClass()
{
  std::function<Object(const Class::NamedParameters &)> make_object_function;
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