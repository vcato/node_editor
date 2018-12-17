#include "streamexecutor.hpp"

#include "anyio.hpp"


void StreamExecutor::print(const std::vector<Any> &value)
{
  printOn(stream,value,/*indent_level*/0);
}


void StreamExecutor::print(const Any &value)
{
  printOn(stream,value,/*indent_level*/0);
}


void StreamExecutor::executeReturn(const Any& arg)
{
  std::cerr << "return ";
  printOn(std::cerr,arg,/*indent_level*/0);
  std::cerr << "\n";
}
