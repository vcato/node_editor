#include "diagramexecutor.hpp"

#include "anyio.hpp"


void DiagramExecutor::executeShow(const Any& arg)
{
  show_stream << arg << "\n";
}
