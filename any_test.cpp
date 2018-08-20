#include "any.hpp"


using std::string;
using std::vector;
using std::ostream;


static void testObject()
{
  struct Data : Object::Data {
    Data *clone() override
    {
      return new Data(*this);
    }

    Optional<Any> member(const string &/*member_name*/) override
    {
      assert(false);
    }

    void printOn(ostream &) const override
    {
      assert(false);
    }
  };

  auto make_test_object_function = [&](const Class::NamedParameters &){
    return Object(std::make_unique<Data>());
  };

  Class test_class(make_test_object_function);

  std::function<Any(const string &member_name)> make_member_function;
  Any a{ Object(std::make_unique<Data>()) };
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


static void testFunction()
{
  bool function_was_called = false;

  std::function<Optional<Any>(const vector<Any> &)> f =
    [&](const vector<Any> &) -> Optional<Any>
    { function_was_called = true; return {5}; };
  Any a(Function{f});
  assert(a.isFunction());
  Optional<Any> maybe_result = a.asFunction()(vector<Any>{});
  assert(function_was_called);
  assert(*maybe_result==5);
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
  testFunction();
  testMemoryLeak1();
}
