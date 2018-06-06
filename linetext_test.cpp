#include "linetext.hpp"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>


using std::ostringstream;
using std::string;
using std::vector;


static float lineTextValue(const char *text,float input_value)
{
  ostringstream stream;
  StreamExecutor executor = {stream};
  evaluateLineText(text,vector<float>{input_value},executor);
  float result = executor.output_value;
  string output = stream.str();
  assert(output=="");
  return result;
}


static float lineTextValue(const char *text,vector<float> input_values)
{
  ostringstream stream;
  StreamExecutor executor = {stream};
  evaluateLineText(text,input_values,executor);
  float result = executor.output_value;
  string output = stream.str();
  assert(output=="");
  return result;
}


static bool lineTextHasInput(const string &text)
{
  return lineTextInputCount(text)>0;
}


namespace {
struct FakeExecutor : Executor {
  ostringstream stream;

  virtual void executeShow(float)
  {
    assert(false);
  }

  virtual void executeReturn(float arg)
  {
    stream << "return(" << arg << ")\n";
  }

  virtual void output(float)
  {
  }
};
}


static float lineTextValue(const string &line_text)
{
  ostringstream dummy_stream;
  float input_value = 0;
  StreamExecutor executor = {dummy_stream};
  evaluateLineText(line_text,vector<float>{input_value},executor);
  return executor.output_value;
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
  assert(lineTextValue("t")==0);
  assert(lineTextValue("$",5)==5);
  assert(lineTextValue("")==0);
  assert(lineTextValue("$+$",{1,2})==3);

  {
    FakeExecutor executor;
    evaluateLineText("return 3",{},executor);
    string execution = executor.stream.str();
    assert(execution=="return(3)\n");
  }
}
