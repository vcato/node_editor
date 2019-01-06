#include "charmapperobjects.hpp"

#include <iostream>
#include <sstream>
#include "any.hpp"
#include "anyio.hpp"


using std::cerr;
using std::make_unique;
using std::ostringstream;
using std::string;


static string valueString(const Any &value)
{
  ostringstream stream;
  stream << value << "\n";
  string result_string = stream.str();
  return result_string;
}


static void testBodyLinkObject()
{
  BodyLink body_link;
  Any value = Object{make_unique<BodyLinkObjectData>(body_link)};

  string result_string = valueString(value);

  string expected_result_string =
    "BodyLink {\n"
    "  scene_name: None\n"
    "  body_name: None\n"
    "}\n";

  if (result_string!=expected_result_string) {
    cerr << "result_string: " << result_string << "\n";
    cerr << "expected_result_string: " << expected_result_string << '\n';
  }

  assert(result_string==expected_result_string);
}


static void testPrintingPosExprObject()
{
  BodyLink body_link;
  Object object(make_unique<PosExprObjectData>(body_link,Point2D{2,3}));
  Any value(std::move(object));
  string expected_result_string =
    "PosExpr {\n"
    "  body: BodyLink {\n"
    "    scene_name: None\n"
    "    body_name: None\n"
    "  }\n"
    "  position: [2,3]\n"
    "}\n";
  assert(valueString(Any(std::move(object)))==expected_result_string);
}


static void testMaybePosExpr()
{
  Any arg;
  ostringstream error_stream;
  Optional<PosExprData> maybe_pos_expr = maybePosExpr(arg,error_stream);
  assert(!maybe_pos_expr);
  string error_string = error_stream.str();
  string expected_error_string = "not a PosExpr\n";
  assert(error_string == expected_error_string);
}


static void testMakingPosExprWhenBodyParameterIsNotABody()
{
  string expected_error_string = "Body parameter is not a body.\n";

  Class::NamedParameters parameters;
  parameters["body"] = 5;
  ostringstream error_stream;
  Optional<Object> maybe_object =
    posExprClass().maybeMakeObject(parameters,error_stream);
  assert(!maybe_object);
  string error_string = error_stream.str();
  assert(error_string == expected_error_string);
}


int main()
{
  testPrintingPosExprObject();
  testBodyLinkObject();
  testMaybePosExpr();
  testMakingPosExprWhenBodyParameterIsNotABody();
}
