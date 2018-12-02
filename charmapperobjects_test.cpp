#include "charmapperobjects.hpp"

#include <iostream>
#include <sstream>
#include "any.hpp"


#define ADD_TEST 0

using std::cerr;
using std::make_unique;
using std::ostringstream;
using std::string;


static void testBodyLinkObject()
{
  BodyLink body_link;
  Any value = Object{make_unique<BodyLinkObjectData>(body_link)};
  ostringstream stream;
  stream << value << "\n";
  string result_string = stream.str();
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


#if ADD_TEST
static void printOn2(std::ostream &stream,const Object &object)
{
  const Object::Data &data = object.data();

  stream << data.typeName() << " {\n";

  // We need a way to handle indentation
  for (const auto &member_name : data.memberNames()) {
    stream << "  " << member_name << ": ";
    Any member_value = *data.maybeMember(member_name);
    ::printOn(stream,member_value);
    stream << "\n";
  }

  stream << "}";
}
#endif


#if ADD_TEST
static void testPrintingPosExprObject()
{
  BodyLink body_link;
  Object body_link_object(make_unique<PosExprObjectData>(body_link,Point2D{2,3}));
  Any value(std::move(body_link_object));
  // value.asObject().maybeMember("body")->asObject().data();
  // cerr << *value.asObject().maybeMember("body") << "\n";
  printOn2(cerr,value.asObject());
  // printOn(cerr,value.asObject());
  // cerr << value << "\n";
}
#endif


int main()
{
#if ADD_TEST
  testPrintingPosExprObject();
#endif
  testBodyLinkObject();
}
