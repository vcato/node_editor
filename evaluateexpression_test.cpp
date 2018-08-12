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


static Optional<Any>
  evaluateStringInEnvironment(const string &arg,Environment &environment)
{
  int index = 0;
  StringParser parser{arg,index};
  vector<Any> input_values;
  int input_index = 0;
  ostringstream error_stream;

  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    environment
  };

  return evaluateExpression(data);
}


static string evaluateStringWithError(const string &arg)
{
  Environment environment;
  int index = 0;
  StringParser parser{arg,index};
  vector<Any> input_values;
  int input_index = 0;
  ostringstream error_stream;

  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    environment
  };

  Optional<Any> maybe_result = evaluateExpression(data);
  assert(!maybe_result);
  return error_stream.str();
}


static Optional<Any> evaluateString(const string &arg)
{
  Environment environment;
  return evaluateStringInEnvironment(arg,environment);
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


static void testPosExpr()
{
  Class pos_expr_class = posExprClass();

  {
    Environment environment;

    environment["PosExpr"] = &pos_expr_class;
    Optional<Any> maybe_result =
      evaluateStringInEnvironment("PosExpr()",environment);
    // This fails because the body parameter is required.
    assert(!maybe_result);
  }
}


static void testPosExpr2()
{
  struct SceneObjectData : Object::Data {
    SceneObjectData(Scene::Body *body1_ptr_arg)
    : body1_ptr(body1_ptr_arg)
    {
    }

    virtual Data *clone() { return new SceneObjectData(*this); }

    virtual Optional<Any> member(const std::string &member_name)
    {
      if (member_name=="body1") {
        return {Object(*new BodyObjectData(body1_ptr))};
      }

      assert(false);
    }

    Scene::Body *body1_ptr;
  };

  Class pos_expr_class = posExprClass();
  int x_var_index = 0;
  int y_var_index = 1;
  Scene::FloatMap x_map(x_var_index), y_map(y_var_index);
  Scene::Point2DMap body1_position_map{x_map,y_map};
  Scene::Body body1(body1_position_map);
  Environment environment;
  environment["PosExpr"] = &pos_expr_class;
  environment["scene1"] = Object(*new SceneObjectData(&body1));
  string expr_string = "PosExpr(body=scene1.body1,position=[0,0])";
  Optional<Any> maybe_result =
    evaluateStringInEnvironment(expr_string,environment);
  assert(maybe_result);
  assert(maybe_result->isObject());
  PosExprObjectData *pos_expr_object_data_ptr =
    dynamic_cast<PosExprObjectData*>(maybe_result->asObject().data_ptr);
  assert(pos_expr_object_data_ptr);
}


static void testCallingUnknownFunction()
{
  string error = evaluateStringWithError("f()");
  assert(error=="Unknown name: f\n");
}


static void testObjectMembers()
{
  Point2D point(1.5,2.5);

  struct Data : Object::Data {
    Point2D &point;

    Data(Point2D &point_arg) : point(point_arg) { }

    Optional<Any> member(const string &member_name)
    {
      if (member_name=="x") {
        return {point.x};
      }

      if (member_name=="y") {
        return {point.y};
      }

      return {};
    }

    virtual Data *clone()
    {
      return new Data(*this);
    }
  };

  auto make_point2d_object_function = [&](const Class::NamedParameters &){
    return Object(*new Data{point});
  };

  Class point2d_class(make_point2d_object_function);

  Object point_object{*new Data(point)};

  Environment environment;
  environment["p"] = Any(std::move(point_object));
  Optional<Any> result = evaluateStringInEnvironment("p.x",environment);
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
  testPosExpr2();
}
