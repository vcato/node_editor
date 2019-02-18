#include "fakeexecutor.hpp"

#include "anyio.hpp"

using std::ostream;


void FakeExecutor::executeShow(const Any& value)
{
  printOn(output_stream,value,/*indent_level*/0);
  output_stream << "\n";
}


bool
  FakeExecutor::tryExecuteReturn(
    const Any& arg,
    std::ostream &/*error_stream*/
  )
{
  execution_stream << "return(";
  printOn(execution_stream, arg, /*indent_level*/0);
  execution_stream << ")\n";
  return true;
}
