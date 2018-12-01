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
using std::make_unique;


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
  ostringstream execution_stream;

  void executeShow(const Any&) override
  {
  }

  void executeReturn(const Any& arg) override
  {
    execution_stream << "return(";
    printOn(execution_stream,arg);
    execution_stream << ")\n";
  }

  virtual std::ostream& errorStream() override { return cerr; }
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


static void testInvalid(const string &line_text)
{
  FakeExecutor executor;
  ostringstream error_stream;
  Optional<Any> maybe_result =
    evaluateLineText(line_text,/*input_values*/{},executor,error_stream);
  assert(!maybe_result);
}


static Optional<Any>
  testLineTextWithoutError(const string &text,FakeExecutor &executor)
{
  ostringstream error_stream;
  Optional<Any> result =
    evaluateLineText(text,/*input_values*/{},executor,error_stream);
  assert(error_stream.str()=="");
  return result;
}


static void
  testExecution(const string &line_text,const string &expected_execution)
{
  FakeExecutor executor;
  testLineTextWithoutError(line_text,executor);
  string execution = executor.execution_stream.str();
  assert(execution==expected_execution);
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

  {
    FakeExecutor executor;
    executor.environment["x"] = 5;
    Optional<Any> maybe_result = testLineTextWithoutError("x",executor);
    assert(*maybe_result==5);
  }
  {
    FakeExecutor executor;
    executor.environment["x"] = 5;
    Optional<Any> maybe_result = testLineTextWithoutError("x+6",executor);
    assert(*maybe_result==11);
  }
  {
    string line_text = "$.f()";

    struct TestObjectData : Object::Data {
      virtual Data *clone() { return new auto(*this); }

      Optional<Any> maybeMember(const std::string &member_name) override
      {
        if (member_name=="f") {
          auto f = [](const vector<Any> &) -> Optional<Any> { return {3}; };
          return {Function{f}};
        }
        cerr << "member_name: " << member_name << "\n";
        assert(false);
      }

      virtual void printOn(std::ostream &) const
      {
        assert(false);
      }

      std::vector<std::string> memberNames() const override
      {
        assert(false); // needs test
      }
    };

    FakeExecutor executor;
    TestObjectData object_data;
    vector<Any> input_values;
    input_values.push_back(Object(make_unique<TestObjectData>()));
    ostringstream error_stream;
    Optional<Any> maybe_result =
      evaluateLineText(line_text,input_values,executor,error_stream);
    assert(*maybe_result==3);
  }
}
