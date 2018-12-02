#include "any.hpp"

#include "printindent.hpp"


void Object::printOn(std::ostream &stream,int indent_level) const
{
  assert(data_ptr);

  stream << data_ptr->typeName() << " {\n";

  for (const auto &member_name : data_ptr->memberNames()) {
    printIndent(stream,indent_level+1);
    stream << member_name << ": ";
    Any member_value = data_ptr->member(member_name);
    ::printOn(stream,member_value,indent_level + 1);
    stream << "\n";
  }

  printIndent(stream,indent_level);
  stream << "}";
}
