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


using std::vector;
using std::cerr;
using std::string;
using std::ostringstream;
using std::make_unique;



namespace {
struct Tester {
  Environment environment;
  vector<Any> input_values;
  ostringstream error_stream;
};
}


static vector<Any> makeVector()
{
  vector<Any> result;
  return result;
}


static vector<Any> makeVector(Any a,Any b)
{
  return vector<Any>{std::move(a),std::move(b)};
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
    tester.environment
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
    tester.environment
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
  Tester tester;
  tester.input_values = {1,2};
  int input_index = 0;
  ExpressionEvaluatorData data{
    parser,
    tester.input_values,
    input_index,
    tester.error_stream,
    tester.environment
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
  Tester tester;
  tester.environment["x"] = 5;
  int input_index = 0;
  ExpressionEvaluatorData data{
    parser,
    tester.input_values,
    input_index,
    tester.error_stream,
    tester.environment
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


static void testPosExpr()
{
  Class pos_expr_class = posExprClass();

  {
    Tester tester;

    tester.environment["PosExpr"] = &pos_expr_class;
    Optional<Any> maybe_result =
      evaluateStringWithTester("PosExpr()",tester);
    // This fails because the body parameter is required.
    assert(!maybe_result);
  }
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

  return maybePosExpr(*maybe_result);
}


static void testPosExpr2()
{
  struct SceneObjectData : Object::Data {
    SceneObjectData(BodyLink body1_link_arg)
    : body1_link(body1_link_arg)
    {
    }

    Data *clone() override { return new SceneObjectData(*this); }

    Optional<Any> maybeMember(const std::string &member_name) override
    {
      if (member_name=="body1") {
        return {bodyObject(body1_link)};
      }

      assert(false);
    }

    void printOn(std::ostream &) const override
    {
      assert(false);
    }

    std::vector<std::string> memberNames() const override
    {
      assert(false); // needs test
    }

    BodyLink body1_link;
  };

  Class pos_expr_class = posExprClass();
  int x_var_index = 0;
  int y_var_index = 1;
  Scene::FloatMap x_map(x_var_index), y_map(y_var_index);
  Scene::Point2DMap body1_position_map{x_map,y_map};
  Scene::Body body1(body1_position_map);
  Tester tester;
  Environment &environment = tester.environment;
  environment["PosExpr"] = &pos_expr_class;
  Scene scene;
  BodyLink body1_link(&scene,&body1);
  environment["scene1"] = Object(make_unique<SceneObjectData>(body1_link));
  string expr_string = "PosExpr(body=scene1.body1,pos=[0,0])";
  Optional<PosExprData> maybe_pos_expr =
    evaluatePosExprExpression(expr_string,tester);
  PosExprData pos_expr = *maybe_pos_expr;
  assert(pos_expr.body_link==body1_link);
  assert(pos_expr.position==Point2D(0,0));
}


static void testCallingUnknownFunction()
{
  string error = evaluateStringWithError("f()");
  assert(error=="Unknown name: f\n");
}


static void testCallingMemberFunctionWithNoArguments()
{
  string expression = "obj.f()";

  struct TestObjectData : Object::Data {
    Data *clone() override { return new auto(*this); }

    Optional<Any> maybeMember(const std::string &member_name) override
    {
      if (member_name=="f") {
        auto f = [](const vector<Any> &) -> Optional<Any> { return {5}; };
        Function::FunctionMember f_member(f);
        return Any(Function{f_member});
      }
      else {
        cerr << "member_name: " << member_name << "\n";
        assert(false);
      }
    }

    void printOn(std::ostream &) const override
    {
      assert(false);
    }

    std::vector<std::string> memberNames() const override
    {
      assert(false); // needs test
    }
  };

  Any obj = Object(make_unique<TestObjectData>());

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

  struct TestObjectData : Object::Data {
    Data *clone() override { return new auto(*this); }

    Optional<Any> maybeMember(const std::string &member_name) override
    {
      if (member_name=="f") {
        auto f = [](const vector<Any> &arg) -> Optional<Any> {
          assert(arg.size()==1);
          return arg[0];
        };

        Function::FunctionMember f_member(f);
        return Any(Function{f_member});
      }
      else {
        cerr << "member_name: " << member_name << "\n";
        assert(false);
      }
    }

    void printOn(std::ostream &) const override
    {
      assert(false);
    }

    std::vector<std::string> memberNames() const override
    {
      assert(false); // needs test
    }
  };

  tester.input_values = {
    Object(make_unique<TestObjectData>()),
    6
  };

  Optional<Any> result = evaluateStringWithTester(expression,tester);
  assert(result);
  assert(*result == 6);
}


static void testObjectMembers()
{
  Point2D point(1.5,2.5);

  auto make_point2d_object_function = [&](const Class::NamedParameters &){
    return makePoint2DObject(point);
  };

  Class point2d_class(make_point2d_object_function);

  Object point_object = makePoint2DObject(point);

  Tester tester;
  tester.environment["p"] = Any(std::move(point_object));
  Optional<Any> result = evaluateStringWithTester("p.x",tester);
  assert(result->asFloat()==1.5);
}


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
  testPosExpr();
  testObjectMembers();
  testCallingUnknownFunction();
  testCallingMemberFunctionWithNoArguments();
  testCallingMemberFunctionWithArgument();
  testPosExpr2();
}
