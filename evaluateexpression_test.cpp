#include "evaluateexpression.hpp"

#include <vector>
#include <iostream>
#include <sstream>
#include "environment.hpp"


using std::vector;
using std::cerr;
using std::string;
using std::ostringstream;


static vector<Any> makeVector()
{
  vector<Any> result;
  return result;
}


static vector<Any> makeVector(Any a,Any b)
{
  return vector<Any>{std::move(a),std::move(b)};
}


static Optional<Any> evaluateString(const string &arg)
{
  int index = 0;
  StringParser parser{arg,index};
  vector<Any> input_values;
  int input_index = 0;
  ostringstream error_stream;
  Environment environment;

  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    environment
  };

  return evaluateExpression(data);
}


static void testInvalidExpression(const string &expression)
{
  Optional<Any> maybe_result = evaluateString(expression);
  assert(!maybe_result);
}

static void test(const string &expression,const Any &expected_result)
{
  Optional<Any> maybe_result = evaluateString(expression);
  assert(maybe_result);

  if (*maybe_result!=expected_result) {
    cerr << "*maybe_result: " << *maybe_result << "\n";
    cerr << "expected_result: " << expected_result << "\n";
  }

  assert(*maybe_result==expected_result);
}


template <typename T>
static void show(const string &name,const T &value)
{
  cerr << name << ": ";
  printOn(cerr,value);
  cerr << "\n";
}


static void testAddingInputs()
{
  string text = "$+$";
  int index = 0;
  StringParser parser(text,index);
  vector<Any> input_values = {1,2};
  int input_index = 0;
  ostringstream error_stream;
  Environment environment;
  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    environment
  };

  Optional<Any> maybe_result = evaluateExpression(data);
  assert(maybe_result);
  const Any& result = *maybe_result;
  assert(result.isFloat());
  assert(result==3);
}


static void testIdentifier()
{
  string text = "x";
  int index = 0;
  StringParser parser(text,index);
  vector<Any> input_values = {};
  ostringstream error_stream;
  Environment environment;
  environment["x"] = 5;
  int input_index = 0;
  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    environment
  };

  Optional<Any> maybe_result = evaluateExpression(data);

  if (!maybe_result) {
    string error_text = error_stream.str();
    cerr << "error: " << error_text << "\n";
  }

  assert(maybe_result);
  const Any &result = *maybe_result;
  assert(result.asFloat()==5);
}


#if 0
static void testPosEXpr()
{
  Optional<Any> maybe_result = evaluateString("PosExpr()");
}
#endif


int main()
{
  testInvalidExpression("[,2]");
  test("[]",makeVector());
  test("[[],2]",makeVector(makeVector(),2));
  test("[1,2] + [3,4]",makeVector(4,6));
  test("2*3",6);
  test("2*[1,2]",makeVector(2,4));
  test("[1,2]*2",makeVector(2,4));
  testInvalidExpression("2*[[],2]");
  testInvalidExpression("[]*[]");
  testInvalidExpression("2*");
  testInvalidExpression("[[],2] + [3,4]");
  testInvalidExpression("[1,2] + [[],4]");
  testInvalidExpression("[1,2] + [3,4,5]");
  testInvalidExpression("[1,2] + 5");
  testInvalidExpression("[1,2] +");
  test("([1,2] + [2,3])/2",makeVector(1.5,2.5));
  testInvalidExpression("([1,2] + [2,3])/[]");
  testInvalidExpression("([1,2] + [2,3])/");
  testInvalidExpression("([[],2] + [2,3])/2");
  testInvalidExpression("([1,2] + [2,3]");
  testInvalidExpression("[[],2]/2");
  testAddingInputs();
  testIdentifier();
#if 0
  testPosEXpr();
#endif
}
