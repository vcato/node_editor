#include "diagramexecutor.hpp"

#include "anyio.hpp"

using std::cerr;
using std::ostream;


void DiagramExecutor::executeShow(const Any& arg)
{
  show_stream << arg << "\n";
}


bool DiagramExecutor::tryExecuteReturn(const Any& arg,ostream &error_stream)
{
  if (optional_expected_return_type_name) {
    if (arg.typeName() != *optional_expected_return_type_name) {
      assert(&error_stream != &cerr);

      error_stream << "Returning " << arg.typeName()
        << " instead of " << *optional_expected_return_type_name << ".\n";
      return false;
    }
  }

  maybe_return_value = arg;

  return true;
}
