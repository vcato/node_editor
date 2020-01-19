#include "evaluateexpression.hpp"

#include <vector>
#include <iostream>
#include <sstream>
#include "environment.hpp"
#include "point2d.hpp"
#include "maybepoint2d.hpp"
#include "scene.hpp"
#include "sceneobjects.hpp"
#include "charmapperobjects.hpp"
#include "point2dobject.hpp"
#include "anyio.hpp"
#include "objectdatawithfmethod.hpp"
#include "stringutil.hpp"

using std::vector;
using std::cerr;
using std::string;
using std::ostringstream;
using std::unique_ptr;
using std::make_unique;
using std::function;
using std::ostream;


static Class stubClass()
{
  auto make_object_function =
    [&](const Class::NamedParameters &,ostream &/*error_stream*/)
      -> Optional<Object>
    {
      assert(false);
    };

  return Class(make_object_function);
}


namespace {
struct Tester {
  Environment environment;
  vector<Any> input_values;
  ostringstream error_stream;

  string errorOutput() const { return error_stream.str(); }
};
}


static Optional<Any>
  evaluateStringWithTester(const string &arg,Tester &tester)
{
  int index = 0;
  StringParser parser{arg,index};
  int input_index = 0;

  ExpressionEvaluatorData data{
    parser,
    tester.input_values,
    input_index,
    tester.error_stream,
    &tester.environment
  };

  string error_string = tester.error_stream.str();
  assert(error_string=="");

  return evaluateExpression(data);
}


static string evaluateStringWithError(const string &arg)
{
  Tester tester;
  int index = 0;
  StringParser parser{arg,index};
  int input_index = 0;

  ExpressionEvaluatorData data{
    parser,
    tester.input_values,
    input_index,
    tester.error_stream,
    &tester.environment
  };

  Optional<Any> maybe_result = evaluateExpression(data);
  assert(!maybe_result);
  return tester.error_stream.str();
}


static Optional<Any> evaluateString(const string &arg)
{
  Tester tester;
  return evaluateStringWithTester(arg,tester);
}


static void testInvalidExpression(const string &expression)
{
  Tester tester;
  Optional<Any> maybe_result = evaluateStringWithTester(expression,tester);
  assert(!maybe_result);
  assert(tester.errorOutput()!="");
}


static void
  testInvalidExpression(
    const string &expression,
    const string &expected_error_message
  )
{
  Tester tester;
  Optional<Any> maybe_result = evaluateStringWithTester(expression,tester);
  assert(!maybe_result);
  bool error_output_is_correct =
    startsWith(tester.errorOutput(),expected_error_message);

  if (!error_output_is_correct) {
    cerr << "Error output: " << tester.errorOutput() << "\n";
    cerr << "expected    : " << expected_error_message << "\n";
  }

  assert(error_output_is_correct);
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
  Tester tester;
  tester.input_values = {1,2};
  int input_index = 0;
  ExpressionEvaluatorData data{
    parser,
    tester.input_values,
    input_index,
    tester.error_stream,
    &tester.environment
  };

  Optional<Any> maybe_result = evaluateExpression(data);
  assert(maybe_result);
  const Any& result = *maybe_result;
  assert(result.isFloat());
  assert(result==3);
}


static void testSubtractingInputs()
{
  string text = "$-$+[0,10]";

  int index = 0;
  StringParser parser(text,index);
  Tester tester;
  tester.input_values = {makeVector(48,0),makeVector(0,0)};
  int input_index = 0;
  ExpressionEvaluatorData data{
    parser,
    tester.input_values,
    input_index,
    tester.error_stream,
    &tester.environment
  };

  Optional<Any> maybe_result = evaluateExpression(data);
  assert(maybe_result);
  const Any& result = *maybe_result;
  assert(result==makeVector(48,10));
}


static void testIdentifier()
{
  string text = "x";
  int index = 0;
  StringParser parser(text,index);
  Tester tester;
  tester.environment["x"] = 5;
  int input_index = 0;
  ExpressionEvaluatorData data{
    parser,
    tester.input_values,
    input_index,
    tester.error_stream,
    &tester.environment
  };

  Optional<Any> maybe_result = evaluateExpression(data);

  if (!maybe_result) {
    string error_text = tester.error_stream.str();
    cerr << "error: " << error_text << "\n";
  }

  assert(maybe_result);
  const Any &result = *maybe_result;
  assert(result.asFloat()==5);
}


static Optional<PosExprData>
  evaluatePosExprExpression(
    const string &expr_string,
    Tester &tester
  )
{
  Optional<Any> maybe_result =
    evaluateStringWithTester(expr_string,tester);

  if (!maybe_result) {
    return {};
  }

  return maybePosExpr(*maybe_result,tester.error_stream);
}


static void testPosExpr()
{
  Class pos_expr_class = posExprClass();
  int x_var_index = 0;
  int y_var_index = 1;
  Scene scene;
  Scene::FloatMap x_map(x_var_index), y_map(y_var_index);
  Scene::Point2DMap body1_position_map{x_map,y_map};
  Scene::Body &body1 = scene.addBody("body1",body1_position_map);
  scene.displayFrame() = Scene::Frame(2);
  Tester tester;
  Environment &environment = tester.environment;
  environment["PosExpr"] = &pos_expr_class;
  environment["scene1"] = Object(make_unique<SceneObjectData>(scene));
  string expr_string = "PosExpr(body=scene1.body1,pos=[0,0])";
  Optional<PosExprData> maybe_pos_expr =
    evaluatePosExprExpression(expr_string,tester);
  PosExprData pos_expr = *maybe_pos_expr;
  assert(pos_expr.body_link==BodyLink(&scene,&body1));
  assert(pos_expr.position==Point2D(0,0));
}


static void testPosExprWithNoParameters()
{
  Class pos_expr_class = posExprClass();
  Tester tester;
  tester.environment["PosExpr"] = &pos_expr_class;
  Optional<Any> maybe_result =
    evaluateStringWithTester("PosExpr()",tester);
  // This fails because the body parameter is required.
  assert(!maybe_result);
}


static void testPosExprWithBadBody()
{
  Class pos_expr_class = posExprClass();
  Tester tester;
  tester.environment["PosExpr"] = &pos_expr_class;
  Optional<Any> maybe_result =
    evaluateStringWithTester("PosExpr(body=[,])",tester);
  // This fails because the body parameter isn't parsable.
  assert(!maybe_result);
}


static void testPosExprWithUnknownParameter()
{
  Class pos_expr_class = posExprClass();
  Tester tester;
  tester.environment["PosExpr"] = &pos_expr_class;
  Optional<Any> maybe_result =
    evaluateStringWithTester("PosExpr(blah=5)",tester);
  assert(!maybe_result);
  assert(tester.error_stream.str()=="Unknown parameter 'blah'\n");
}


static void testCallingUnknownFunction()
{
  string error = evaluateStringWithError("f()");
  assert(error=="Unknown name: f\n");
}


static void testCallingFunctionWithMissingCloseParen()
{
  string expression = "f(";

  Tester tester;
  tester.environment["f"] = Function();
  evaluateStringWithTester(expression,tester);
}


static void testCallingMemberFunctionWithNoArguments()
{
  string expression = "obj.f()";

  auto f = [](const vector<Any> &) -> Optional<Any> { return {5}; };

  unique_ptr<ObjectDataWithFMethod> object_data_ptr =
    make_unique<ObjectDataWithFMethod>(f);

  Any obj = Object(std::move(object_data_ptr));

  Tester tester;
  tester.environment["obj"] = obj;
  Optional<Any> maybe_result =
    evaluateStringWithTester(expression,tester);
  assert(maybe_result);
  assert((*maybe_result)==5);
}


static void testCallingMemberFunctionWithArgument()
{
  string expression = "$.f($)";
  Tester tester;

  auto f =
    [](const vector<Any> &arg) -> Optional<Any> {
      assert(arg.size()==1);
      return arg[0];
    };

  unique_ptr<ObjectDataWithFMethod> object_data_ptr =
    make_unique<ObjectDataWithFMethod>(f);

  tester.input_values = {
    Object(std::move(object_data_ptr)),
    6
  };

  Optional<Any> result = evaluateStringWithTester(expression,tester);
  assert(result);
  assert(*result == 6);
}


static void testCallingFunctionWithTwoArguments()
{
  string expression = "f(1,2)";

  auto f =
    [](const vector<Any> &args) -> Optional<Any> {
      assert(args.size() == 2);
      assert(args[0].asFloat() == 1);
      assert(args[1].asFloat() == 2);
      return Any(3);
    };

  Tester tester;
  tester.environment["f"] = Function{f};
  Optional<Any> result = evaluateStringWithTester(expression, tester);
  assert(result->asFloat() == 3);
}


static void testCallingFunctionWithVariableParameter()
{
  string expression = "f(a)";

  auto f =
    [](const vector<Any> &args) -> Optional<Any> {
      assert(args.size() == 1);
      assert(args[0].asFloat() == 5);
      return Any(6);
    };

  Tester tester;
  tester.environment["a"] = 5;
  tester.environment["f"] = Function{f};
  Optional<Any> result = evaluateStringWithTester(expression, tester);
  assert(result->asFloat() == 6);
}


static void testBodyPositionWithLocal()
{
  Tester tester;
  Scene scene;
  Scene::Body &body = scene.addBody("body");
  scene.displayFrame() = Scene::Frame(2);
  tester.environment["body"] = makeBodyObject(BodyLink(&scene,&body));
  Optional<Any> maybe_result =
    evaluateStringWithTester("body.pos([1,2])",tester);
  assert(maybePoint2D(*maybe_result));
}


static void testObjectConstructionWithMissingComma()
{
  Class test_class = stubClass();
  Tester tester;
  Environment &environment = tester.environment;
  environment["Stub"] = &test_class;
  string expr_string = "Stub(x=5 6)";
  Optional<Any> maybe_result =
    evaluateStringWithTester(expr_string,tester);

  assert(!maybe_result);
  assert(tester.error_stream.str()=="Missing ','\n");
}


static void testPoint2DMembers()
{
  Point2D point(1.5,2.5);

  Any point_object = makePoint2DObject(point);

  Tester tester;
  tester.environment["p"] = Any(std::move(point_object));
  {
    Optional<Any> result = evaluateStringWithTester("p.x",tester);
    assert(result->asFloat()==1.5);
  }
  {
    Optional<Any> result = evaluateStringWithTester("p.y",tester);
    assert(result->asFloat()==2.5);
  }
  {
    Optional<Any> result = evaluateStringWithTester("p.z",tester);
    assert(!result);
  }
}


int main()
{
  test("[]",makeVector());
  test("[[],2]",makeVector(makeVector(),2));
  test("[1,2] + [3,4]",makeVector(4,6));
  test("[1,2] - [3,5]",makeVector(-2,-3));
  test("2-3",-1);
  test("2+3-1",4);
  test("[1,2]-[3,4]+[5,6]",makeVector(3,4));
  test("2*3",6);
  test("2*[1,2]",makeVector(2,4));
  test("[1,2]*2",makeVector(2,4));
  test("([1,2] + [2,3])/2",makeVector(1.5,2.5));
  test("2*3+4",2*3+4);
  test("2*3 + 4*5", 2*3 + 4*5);

  testInvalidExpression("[,2]");
  testInvalidExpression("2*[[],2]");
  testInvalidExpression("[]*[]");
  testInvalidExpression("2*","Unexpected end of expression.");
  testInvalidExpression("[[],2] + [3,4]");
  testInvalidExpression("[1,2] + [[],4]");
  testInvalidExpression("[1,2] + [3,4,5]");
  testInvalidExpression("[1,2] + 5");
  testInvalidExpression("[1,2] +");
  testInvalidExpression("1.x");
  testInvalidExpression("[1,2] - 3");
  testInvalidExpression("[1,2] - [3]");
  testInvalidExpression("[1,2] - [[],3]");
  testInvalidExpression("[[],2] - [3,3]");
  testInvalidExpression("([1,2] + [2,3])/[]");
  testInvalidExpression("([1,2] + [2,3])/");
  testInvalidExpression("([[],2] + [2,3])/2");
  testInvalidExpression("([1,2] + [2,3]");
  testInvalidExpression("[[],2]/2");

  testAddingInputs();
  testSubtractingInputs();
  testIdentifier();
  testPosExpr();
  testPosExprWithNoParameters();
  testPosExprWithBadBody();
  testPosExprWithUnknownParameter();
  testPoint2DMembers();
  testCallingUnknownFunction();
  testCallingFunctionWithMissingCloseParen();
  testCallingMemberFunctionWithNoArguments();
  testCallingMemberFunctionWithArgument();
  testCallingFunctionWithTwoArguments();
  testCallingFunctionWithVariableParameter();
  testBodyPositionWithLocal();
  testObjectConstructionWithMissingComma();
}
