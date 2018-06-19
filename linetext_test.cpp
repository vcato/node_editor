#include "linetext.hpp"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "streamexecutor.hpp"


using std::ostringstream;
using std::string;
using std::vector;
using std::cerr;
using std::map;


static Any lineTextValue(const char *text,const vector<Any> &input_values)
{
  ostringstream stream;
  StreamExecutor executor = {stream};
  Optional<Any> maybe_result = evaluateLineText(text,input_values,executor);
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
  ostringstream stream;
  map<string,Any> environment;

  void executeShow(const Any&) override
  {
  }

  void executeReturn(const Any& arg) override
  {
    stream << "return(";
    printOn(stream,arg);
    stream << ")\n";
  }

  Optional<Any> variableValue(const std::string &name) const override
  {
    auto iter = environment.find(name);

    if (iter==environment.end()) {
      return {};
    }

    return iter->second;
  }
};
}


static Any lineTextValue(const string &line_text)
{
  ostringstream dummy_stream;
  StreamExecutor executor = {dummy_stream};
  Optional<Any> maybe_result = evaluateLineText(line_text,{},executor);

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


static void testInvalid(const string &line_text)
{
  FakeExecutor executor;
  Optional<Any> maybe_result = evaluateLineText(line_text,{},executor);
  assert(!maybe_result);
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

  testInvalid("show");
  testInvalid("show(5");
  testInvalid("show()");
  testInvalid("show(5)x");

  {
    FakeExecutor executor;
    evaluateLineText("return 3",{},executor);
    string execution = executor.stream.str();
    assert(execution=="return(3)\n");
  }

  {
    FakeExecutor executor;
    evaluateLineText("return [1,2]",{},executor);
    string execution = executor.stream.str();
    assert(execution=="return([1,2])\n");
  }

  {
    FakeExecutor executor;
    Optional<Any> maybe_result = evaluateLineText("return [",{},executor);
    assert(!maybe_result);
  }

  {
    FakeExecutor executor;
    executor.environment["x"] = 5;
    Optional<Any> maybe_result = evaluateLineText("x",{},executor);
    assert(maybe_result);
    assert(*maybe_result==5);
  }
}
