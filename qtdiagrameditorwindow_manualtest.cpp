#include "qtdiagrameditorwindow.hpp"

#include <iostream>
#include <sstream>
#include <QApplication>
#include "evaluatediagram.hpp"
#include "diagramexecutor.hpp"
#include "anyio.hpp"

using std::cerr;
using std::ostringstream;


static void
  reevaluateDiagram(
    const Diagram &diagram,
    DiagramEvaluationState &diagram_state
  )
{
  ostringstream show_stream;
  ostringstream error_stream;
  DiagramExecutionContext context = {show_stream,error_stream};
  DiagramExecutor executor(context,context.parent_environment_ptr);
  diagram_state.clear();
  evaluateDiagram(diagram,executor,diagram_state);

  cerr << "result: " << executor.maybe_return_value << "\n";
  cerr << "Done evaluateDiagram()\n";
  cerr << "errors: " << error_stream.str() << "\n";
  cerr << "show: " << show_stream.str() << "\n";
}


namespace {
struct HolderStub : ObservedDiagram::Holder {
  void notifyDiagramUnobserved(Diagram &) override {}
  void notifyDiagramChanged(Diagram &) override {}
};
}


int main(int argc,char **argv)
{
  QApplication app(argc,argv);
  Diagram diagram;
  HolderStub holder_stub;
  ObservedDiagram observed_diagram(diagram,holder_stub);

  // The window needs to be declared after the observed diagram so that
  // the window will be destroyed before the observed diagram that it is
  // using.
  QtDiagramEditorWindow window;

  auto diagram_changed_function = [&](){
    observed_diagram.maybe_diagram_state.emplace();
    reevaluateDiagram(
      observed_diagram.diagram,
      *observed_diagram.maybe_diagram_state
    );
    observed_diagram.notifyObserversThatDiagramStateChanged();
  };

  auto diagram_observer_ptr =
    std::make_unique<ObservedDiagram::Observer>(
      observed_diagram,
      diagram_changed_function
    );
  window.setDiagramObserver(std::move(diagram_observer_ptr));

  window.show();
  app.exec();
}
