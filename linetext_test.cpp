#include "linetext.hpp"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "streamexecutor.hpp"
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


static Any lineTextValue(const char *text,const vector<Any> &input_values)
{
  ostringstream stream;
  StreamExecutor executor = {stream,cerr};
  ostringstream error_stream;
  Optional<Any> maybe_result =
    evaluateLineText(text,input_values,executor,error_stream);
  assert(maybe_result);
  string output = stream.str();
  assert(output=="");
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


namespace {
struct FakeExecutor : Executor {
  ostringstream &execution_stream;

  using Executor::Executor;

  void executeShow(const Any&) override
  {
  }

  bool tryExecuteReturn(const Any& arg,ostream &/*error_stream*/) override
  {
    execution_stream << "return(";
    printOn(execution_stream, arg, /*indent_level*/0);
    execution_stream << ")\n";
    return true;
  }

  std::ostream& errorStream() override { return cerr; }

  FakeExecutor(
    const Environment *parent_environment_ptr,
    ostringstream &execution_stream_arg
  )
  : Executor(parent_environment_ptr),
    execution_stream(execution_stream_arg)
  {
  }
};
}


static Any lineTextValue(const string &line_text)
{
  ostringstream dummy_stream;
  StreamExecutor executor = {dummy_stream,cerr};
  ostringstream error_stream;
  Optional<Any> maybe_result =
    evaluateLineText(line_text,/*input_values*/{},executor,error_stream);

  if (!maybe_result) {
    return Any();
  }

  return std::move(*maybe_result);
}


static vector<Any> makeVector(float a,float b)
{
  vector<Any> result;
  result.push_back(a);
  result.push_back(b);
  return result;
}


namespace {
struct Tester {
  ostringstream execution_stream;
  Environment environment;
  vector<Any> input_values;
};
}


static void testInvalid(const string &line_text)
{
  Tester tester;
  FakeExecutor executor(&tester.environment,tester.execution_stream);
  ostringstream error_stream;
  Optional<Any> maybe_result =
    evaluateLineText(line_text,tester.input_values,executor,error_stream);
  assert(!maybe_result);
}


static Any testLineTextWithoutError(const string &text,Tester &tester)
{
  FakeExecutor executor(&tester.environment,tester.execution_stream);
  ostringstream error_stream;
  Optional<Any> result =
    evaluateLineText( text, tester.input_values, executor, error_stream );
  assert(error_stream.str() == "");
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
