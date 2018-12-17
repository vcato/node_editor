#include "anyio.hpp"

#include "printindent.hpp"


static void
  printObjectOn(std::ostream &stream,const Object &object,int indent_level)
{
  const auto &data = object.data();

  stream << data.typeName() << " {\n";

  for (const auto &member_name : data.memberNames()) {
    printIndent(stream,indent_level+1);
    stream << member_name << ": ";
    Any member_value = data.member(member_name);
    ::printOn(stream,member_value,indent_level + 1);
    stream << "\n";
  }

  printIndent(stream,indent_level);
  stream << "}";
}


template <>
void printOn(std::ostream &stream,const Object &object,int indent_level)
{
  printObjectOn(stream,object,indent_level);
}
