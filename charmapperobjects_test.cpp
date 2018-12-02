#include "charmapperobjects.hpp"

#include <iostream>
#include <sstream>
#include "any.hpp"


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


int main()
{
  testPrintingPosExprObject();
  testBodyLinkObject();
}
