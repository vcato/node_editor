#include "any.hpp"
#include "printon.hpp"
#include "basicvariantio.hpp"


template <>
inline void printOn(std::ostream &stream,const float &arg,int /*indent_level*/)
{
  stream << arg;
}


template <>
inline void printOn(std::ostream &stream,const Any::Void &,int /*indent_level*/)
{
  stream << "None";
}


template <>
inline void
  printOn(std::ostream &stream,const std::string &arg,int /*indent_level*/)
{
  stream << '"' << arg << '"';
}


template <>
inline void
  printOn(std::ostream &stream,const std::vector<Any> &arg,int indent_level)
{
  stream << "[";

  auto iter = arg.begin();

  if (iter!=arg.end()) {
    printOn(stream,*iter,indent_level);
    ++iter;
  }

  while (iter!=arg.end()) {
    stream << ",";
    printOn(stream,*iter,indent_level);
    ++iter;
  }

  stream << "]";
}


template <>
void printOn(std::ostream &stream,const Object &object,int indent_level);

template <>
inline void printOn(std::ostream &stream,const Function &,int /*indent_level*/)
{
  stream << "Function()";
}


template <>
inline void printOn(std::ostream &stream,Class *const &,int /*indent_level*/)
{
  stream << "Class()\n";
}


inline std::ostream& operator<<(std::ostream &stream,const Any &arg)
{
  // This printOn() is defined in basicvariant.hpp
  printOn(stream,arg,/*indent_level*/0);
  return stream;
}
