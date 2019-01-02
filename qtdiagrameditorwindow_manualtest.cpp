#include "qtdiagrameditorwindow.hpp"

#include <iostream>
#include <sstream>
#include <QApplication>
#include "evaluatediagram.hpp"
#include "diagramexecutor.hpp"
#include "anyio.hpp"

using std::cerr;
using std::ostringstream;


static void handleDiagramChanged(Diagram &diagram)
{
  ostringstream show_stream;
  ostringstream error_stream;
  DiagramExecutionContext context = {show_stream,error_stream};
  DiagramExecutor executor(context);
  DiagramState diagram_state;
  evaluateDiagram(diagram,executor,diagram_state);

  cerr << "result: " << executor.maybe_return_value << "\n";
  cerr << "Done evaluateDiagram()\n";
  cerr << "errors: " << error_stream.str() << "\n";
  cerr << "show: " << show_stream.str() << "\n";
  // At this point, we could do editor.setDiagramState(diagram_state)
  // so that the editor could do things like showing the output values
  // or the evaluation errors.
}



int main(int argc,char **argv)
{
  QApplication app(argc,argv);
  Diagram diagram;
  QtDiagramEditorWindow window;
  window.diagramChangedCallback() = [&]{ handleDiagramChanged(diagram); };
  window.setDiagramPtr(&diagram);
  window.show();
  app.exec();
}
