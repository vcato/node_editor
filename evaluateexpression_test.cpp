#include "evaluateexpression.hpp"

#include <vector>
#include <iostream>


using std::vector;
using std::cerr;
using std::string;


static Optional<Any> evaluateString(const string &arg)
{
  int index = 0;
  Parser parser{arg,index};
  vector<Any> input_values;
  int input_index = 0;
  return evaluateExpression(parser,input_values,input_index);
}


static void testInvalidExpression(const string &expression)
{
  Optional<Any> maybe_result = evaluateString(expression);
  assert(!maybe_result);
}


static vector<Any> make_vector(float a,float b)
{
  vector<Any> result;
  result.push_back(a);
  result.push_back(b);
  return result;
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
static void show(const string &name,const T &value)
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
    assert(maybe_result->asVector()==make_vector(4,6));
  }

  testInvalidExpression("[[],2] + [3,4]");
  testInvalidExpression("[1,2] + [[],4]");
  testInvalidExpression("[1,2] + [3,4,5]");
  testInvalidExpression("[1,2] + 5");
  testInvalidExpression("[1,2] +");
}


static void testVectorAverage()
{
  {
    Optional<Any> maybe_result = evaluateString("([1,2] + [2,3])/2");
    assert(maybe_result);
    assert(maybe_result->asVector()==make_vector(1.5,2.5));
  }
  testInvalidExpression("([1,2] + [2,3])/[]");
  testInvalidExpression("([1,2] + [2,3])/");
  testInvalidExpression("([[],2] + [2,3])/2");
  testInvalidExpression("([1,2] + [2,3]");
  testInvalidExpression("[[],2]/2");
}


static void testAddingInputs()
{
  string text = "$+$";
  int index = 0;
  Parser parser(text,index);
  vector<Any> input_values = {1,2};
  int input_index = 0;
  Optional<Any> maybe_result =
    evaluateExpression(
      parser,
      input_values,
      input_index
    );
  assert(maybe_result);
  const Any& result = *maybe_result;
  assert(result.isFloat());
  assert(result==3);
}


int main()
{
  testListWithInvalidElement();
  testEmptyList();
  testNestedVectors();
  testAddingVectors();
  testVectorAverage();
  testAddingInputs();
}
