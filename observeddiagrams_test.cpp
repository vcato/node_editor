#include "observeddiagrams.hpp"

#include <cassert>
#include "diagram.hpp"

using ObservedDiagram = ObservedDiagrams::ObservedDiagram;
using DiagramObserverPtr = ObservedDiagrams::DiagramObserverPtr;


namespace {
struct FakeWrapper {
  Diagram &diagram;
  ObservedDiagrams &observed_diagrams;

  FakeWrapper(Diagram &diagram_arg,ObservedDiagrams &observed_diagrams_arg)
  : diagram(diagram_arg), observed_diagrams(observed_diagrams_arg)
  {
  }

  DiagramObserverPtr makeDiagramObserver()
  {
    return observed_diagrams.makeObserver(diagram);
  }
};
}


namespace {
struct FakeDiagramEditor {
  int redraw_count = 0;
  DiagramObserverPtr diagram_observer_ptr;

  void diagramStateChanged()
  {
    redraw();
  }

  void setDiagramObserver(DiagramObserverPtr &&observer_ptr)
  {
    diagram_observer_ptr = std::move(observer_ptr);

    if (diagram_observer_ptr) {
      diagram_observer_ptr->diagram_state_changed_callback =
        [&] { diagramStateChanged(); };
    }
  }

  void redraw()
  {
    ++redraw_count;
  }
};
}


namespace {
struct FakeDiagramEvaluator {
  ObservedDiagrams &observed_diagrams;

  FakeDiagramEvaluator(ObservedDiagrams &observed_diagrams_arg)
  : observed_diagrams(observed_diagrams_arg)
  {
  }

  void evaluate(Diagram &diagram)
  {
    ObservedDiagram *maybe_observed_diagram =
      observed_diagrams.findObservedDiagramFor(diagram);

    if (!maybe_observed_diagram) {
      DiagramState temporary_diagram_state;
      // evaluateDiagram(diagram,temporary_diagram_state);
      return;
    }

    ObservedDiagram &observed_diagram = *maybe_observed_diagram;

    // evaluateDiagram(diagram,observed_diagram.diagram_state);
    observed_diagram.notifyDiagramStateChanged();
  }
};
}


static void testWithOneObserver()
{
  ObservedDiagrams observed_diagrams;
  Diagram diagram;
  FakeWrapper wrapper(diagram,observed_diagrams);
  FakeDiagramEditor editor;

  editor.setDiagramObserver(wrapper.makeDiagramObserver());
  assert(editor.redraw_count==0);

  {
    FakeDiagramEvaluator evaluator(observed_diagrams);
    evaluator.evaluate(diagram);
  }

  assert(editor.redraw_count==1);
  editor.setDiagramObserver(nullptr);

  assert(!observed_diagrams.findObservedDiagramFor(diagram));
}


static void testWithTwoObservers()
{
  ObservedDiagrams observed_diagrams;
  Diagram diagram;
  FakeWrapper wrapper(diagram,observed_diagrams);
  FakeDiagramEditor editor1;
  FakeDiagramEditor editor2;

  editor1.setDiagramObserver(wrapper.makeDiagramObserver());
  editor2.setDiagramObserver(wrapper.makeDiagramObserver());

  assert(editor1.redraw_count==0);
  assert(editor2.redraw_count==0);

  {
    FakeDiagramEvaluator evaluator(observed_diagrams);
    evaluator.evaluate(diagram);
  }

  assert(editor1.redraw_count==1);
  assert(editor2.redraw_count==1);

  editor1.setDiagramObserver(nullptr);
  editor2.setDiagramObserver(nullptr);

  assert(!observed_diagrams.findObservedDiagramFor(diagram));
}


static void testEvaluatingWithNoObservers()
{
  ObservedDiagrams observed_diagrams;
  Diagram diagram;
  FakeWrapper wrapper(diagram,observed_diagrams);

  {
    FakeDiagramEvaluator evaluator(observed_diagrams);
    evaluator.evaluate(diagram);
  }

  assert(!observed_diagrams.findObservedDiagramFor(diagram));
}


int main()
{
  testWithOneObserver();
  testWithTwoObservers();
  testEvaluatingWithNoObservers();
}
