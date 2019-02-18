#include "linetext.hpp"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "fakeexecutor.hpp"
#include "scene.hpp"
#include "sceneobjects.hpp"
#include "maybepoint2d.hpp"
#include "generatename.hpp"
#include "anyio.hpp"
#include "evaluatelinetext.hpp"
#include "objectdatawithfmethod.hpp"

using std::ostringstream;
using std::string;
using std::vector;
using std::cerr;
using std::map;
using std::make_unique;
using std::unique_ptr;
using std::function;
using std::ostream;


namespace {
struct Tester {
  ostringstream execution_stream;
  Environment environment;
  vector<Any> input_values;
  ostringstream output_stream;
  ostringstream error_stream;
  ostringstream debug_stream;

  FakeExecutor
    executor{
      &environment,
      execution_stream,
      output_stream,
      debug_stream
    };

  Optional<Any> evaluate(const string &text)
  {
    // We'll assume that we're not evaluating lines that do assignment
    // right now, so we don't need an environment allocator.
    auto allocate_environment_function =
      [](const Environment *) -> Environment& { assert(false); };

    Optional<Any> maybe_result =
      evaluateLineText(
        text,
        input_values,
        executor,
        error_stream,
        allocate_environment_function
      );
    return maybe_result;
  }

  string errorString()
  {
    return error_stream.str();
  }

  string outputString()
  {
    return output_stream.str();
  }

  string debugString()
  {
    return debug_stream.str();
  }
};
}


static Any lineTextValue(const char *text,vector<Any> input_values_arg)
{
  Tester tester;
  tester.input_values = std::move(input_values_arg);

  Optional<Any> maybe_result = tester.evaluate(text);

  assert(maybe_result);
  assert(tester.outputString() == "");
  assert(tester.debugString() == "");
  return std::move(*maybe_result);
}


static Any lineTextValue(const char *text,const Any& input_value)
{
  return lineTextValue(text,vector<Any>{Any(input_value)});
}


static bool lineTextHasInput(const string &text)
{
  return lineTextInputCount(text)>0;
}


static Any lineTextValue(const string &line_text)
{
  Tester tester;

  Optional<Any> maybe_result = tester.evaluate(line_text);

  if (!maybe_result) {
    return Any();
  }

  assert(tester.debugString() == "");
  assert(tester.outputString() == "");

  return std::move(*maybe_result);
}


static vector<Any> makeVector(float a,float b)
{
  vector<Any> result;
  result.push_back(a);
  result.push_back(b);
  return result;
}


static void testInvalid(const string &line_text)
{
  Tester tester;

  Optional<Any> maybe_result = tester.evaluate(line_text);

  assert(!maybe_result);
}


static Any testLineTextWithoutError(const string &text, Tester &tester)
{
  Optional<Any> result = tester.evaluate(text);

  assert(tester.errorString() == "");
  assert(result);
  return std::move(*result);
}


static void
  testExecution(const string &line_text,const string &expected_execution)
{
  Tester tester;
  testLineTextWithoutError(line_text,tester);
  ostringstream &execution_stream = tester.execution_stream;
  string execution = execution_stream.str();
  assert(execution==expected_execution);
}


template <typename Function>
static Object makeObjectWithFMethod(const Function &f)
{
  unique_ptr<ObjectDataWithFMethod> object_data_ptr =
    make_unique<ObjectDataWithFMethod>(f);
  return Object(std::move(object_data_ptr));
}


static void testCallingMethod()
{
  string line_text = "$.f()";

  auto f = [](const vector<Any> &) -> Optional<Any> { return {3}; };
  Tester tester;
  tester.input_values.push_back(makeObjectWithFMethod(f));
  Any result = testLineTextWithoutError(line_text,tester);
  assert(result == 3);
}


static void testCallingSceneBodyPos()
{
  string expr = "scene.body1.pos()";
  Scene scene;
  scene.addBody("body1");
  Tester tester;
  tester.environment["scene"] = makeSceneObject(scene);
  Any result = testLineTextWithoutError(expr,tester);
  assert(maybePoint2D(result));
}


static void testCase1()
{
  Tester tester;
  tester.input_values = {makeVector(1,0)};
  tester.environment["x"] = 2;
  string text = "return $*x + [1,0]";
  string expected_execution = "return([3,0])\n";

  testLineTextWithoutError(text,tester);

  string execution = tester.execution_stream.str();
  assert(execution == expected_execution);
}


int main()
{
  assert(lineTextHasInput("x=$"));
  assert(lineTextHasInput("$.pos("));
  assert(lineTextHasInput("cos($)"));
  assert(lineTextHasInput("  body=$,"));
  assert(!lineTextHasOutput("let x=5"));
  assert(!lineTextHasOutput("x=$"));
  assert(!lineTextHasOutput(""));
  assert(!lineTextHasOutput(" "));
  assert(!lineTextHasOutput(")"));
  assert(!lineTextHasOutput("return $"));
  assert(lineTextValue("5")==5);
  assert(lineTextValue("t")==Any());
  assert(lineTextValue("$",5)==5);
  assert(lineTextValue("")==Any());
  assert(lineTextValue("$+$",{1,2})==3);
  assert(lineTextValue("[1,2]")==makeVector(1,2));
  assert(lineTextValue("1/2")==0.5);

  testInvalid("show");
  testInvalid("show(5");
  testInvalid("show()");
  testInvalid("show(5)x");
  testInvalid("return [");

  testExecution("return 3","return(3)\n");
  testExecution("return [1,2]","return([1,2])\n");

  testCallingMethod();
  testCallingSceneBodyPos();

  testCase1();
}
