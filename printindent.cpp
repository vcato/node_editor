#include "printindent.hpp"


void printIndent(std::ostream &stream,int indent)
{
  for (int i=0; i!=indent; ++i) {
    stream << "  ";
  }
}
