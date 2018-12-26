#include "diagramexecutor.hpp"

#include <sstream>
#include "evaluatediagram.hpp"


using std::ostringstream;

#define ADD_TEST 0


#if ADD_TEST
static void testDiagramWithNoReturn()
{
  ostringstream show_stream;
  ostringstream error_stream;
  DiagramExecutionContext context = {show_stream,error_stream};
  DiagramExecutor executor(context);
  Diagram diagram;
  evaluateDiagram(diagram,executor);
  assert(!executor.maybe_return_value);
}
#endif


int main()
{
#if ADD_TEST
  testDiagramWithNoReturn();
#endif
}
