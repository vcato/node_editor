#include "printon.hpp"


template <typename Policy>
inline void
  printOn(std::ostream &stream,const BasicVariant<Policy> &arg,int indent_level)
{
  arg.visit([&](auto &arg){ printOn(stream,arg,indent_level); });
}
