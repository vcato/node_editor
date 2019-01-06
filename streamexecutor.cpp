#include "streamexecutor.hpp"

#include "anyio.hpp"

using std::ostream;


void StreamExecutor::print(const std::vector<Any> &value)
{
  printOn(stream,value,/*indent_level*/0);
}


void StreamExecutor::print(const Any &value)
{
  printOn(stream,value,/*indent_level*/0);
}


bool StreamExecutor::tryExecuteReturn(const Any& arg,ostream &/*error_stream*/)
{
  std::cerr << "return ";
  printOn(std::cerr,arg,/*indent_level*/0);
  std::cerr << "\n";
  return true;
}
