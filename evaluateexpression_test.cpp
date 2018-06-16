#include "evaluateexpression.hpp"

#include <vector>
#include <iostream>


using std::vector;
using std::cerr;


#if 1
static vector<Any> make_vector(float a,float b)
{
  vector<Any> result;
  result.push_back(a);
  result.push_back(b);
  return result;
}
#endif


static Optional<Any> evaluateString(const std::string &arg)
{
  int index = 0;
  Parser parser{arg,index};
  vector<float> input_values;
  int input_index = 0;
  return evaluateExpression(parser,input_values,input_index);
}


static void testListWithInvalidElement()
{
  Optional<Any> maybe_result = evaluateString("[,2]");
  assert(!maybe_result);
}


static void testEmptyList()
{
  Optional<Any> maybe_result = evaluateString("[]");
  assert(maybe_result);
  assert(maybe_result->asVector().empty());
}


static void testNestedVectors()
{
  Optional<Any> maybe_result = evaluateString("[[],2]");
  assert(maybe_result);
}


template <typename T>
static void show(const std::string &name,const T &value)
{
  cerr << name << ": ";
  printOn(cerr,value);
  cerr << "\n";
}


static void testAddingVectors()
{
  {
    Optional<Any> maybe_result = evaluateString("[1,2] + [3,4]");
    assert(maybe_result);
    show("result",*maybe_result);
    assert(maybe_result->asVector()==make_vector(4,6));
  }
  {
    Optional<Any> maybe_result = evaluateString("[[],2] + [3,4]");
    assert(!maybe_result);
  }
  {
    Optional<Any> maybe_result = evaluateString("[1,2] + [[],4]");
    assert(!maybe_result);
  }
  {
    Optional<Any> maybe_result = evaluateString("[1,2] + [3,4,5]");
    assert(!maybe_result);
  }
  {
    Optional<Any> maybe_result = evaluateString("[1,2] + 5");
    assert(!maybe_result);
  }
  {
    Optional<Any> maybe_result = evaluateString("[1,2] +");
    assert(!maybe_result);
  }
}


int main()
{
  testListWithInvalidElement();
  testEmptyList();
  testNestedVectors();
  testAddingVectors();
}
