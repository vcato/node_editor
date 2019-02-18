#include "observeddiagrams.hpp"

#include <iostream>


using std::cerr;
using std::make_pair;


ObservedDiagrams::ObservedDiagrams(
  DiagramChangedCallback diagram_changed_callback_arg
)
: diagram_changed_callback(std::move(diagram_changed_callback_arg))
{
}


ObservedDiagrams::~ObservedDiagrams()
{
  assert(observed_diagram_map.empty());
}


DiagramObserverPtr
  ObservedDiagrams::makeObserver(
    Diagram &diagram,
    std::function<void()> diagram_changed_hook
  )
{
  auto iter = observed_diagram_map.find(&diagram);

  if (iter == observed_diagram_map.end()) {
    auto emplace_result =
      observed_diagram_map.emplace(&diagram,ObservedDiagram{diagram,*this});
    assert(/*was_inserted*/emplace_result.second);
    iter = emplace_result.first;
  }

  ObservedDiagram &observed_diagram = iter->second;
  return
    std::make_unique<DiagramObserver>(
      observed_diagram,
      diagram_changed_hook
    );
}


ObservedDiagram *
  ObservedDiagrams::findObservedDiagramFor(const Diagram &diagram)
{
  auto iter = observed_diagram_map.find(&diagram);

  if (iter==observed_diagram_map.end()) {
    return nullptr;
  }

  ObservedDiagram &observed_diagram = iter->second;
  return &observed_diagram;
}


void ObservedDiagrams::notifyDiagramUnobserved(Diagram &diagram)
{
  auto iter = observed_diagram_map.find(&diagram);
  assert(iter!=observed_diagram_map.end());
  observed_diagram_map.erase(iter);
}


void ObservedDiagrams::notifyDiagramChanged(Diagram &diagram)
{
  if (diagram_changed_callback) {
    diagram_changed_callback(diagram);
  }
}
