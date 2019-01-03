#include "observeddiagrams.hpp"

#include <algorithm>

using std::make_unique;


ObservedDiagrams::ObservedDiagram::Observer::Observer(
  ObservedDiagram &observed_diagram_arg
)
: observed_diagram(observed_diagram_arg)
{
  observed_diagram.addObserver(*this);
}


ObservedDiagrams::ObservedDiagram::Observer::~Observer()
{
  observed_diagram.removeObserver(*this);
}


ObservedDiagrams::ObservedDiagram::ObservedDiagram(
  Diagram &diagram_arg,ObservedDiagrams &holder_arg
)
: diagram(diagram_arg),
  holder(holder_arg)
{
}


void ObservedDiagrams::ObservedDiagram::notifyDiagramStateChanged()
{
  for (Observer *observer_ptr : observers) {
    assert(observer_ptr);
    observer_ptr->diagram_state_changed_callback();
  }
}


void ObservedDiagrams::ObservedDiagram::addObserver(Observer &observer)
{
  observers.push_back(&observer);
}


void ObservedDiagrams::ObservedDiagram::removeObserver(Observer &observer)
{
  auto iter =
    std::find(
      observers.begin(),
      observers.end(),
      &observer
    );

  assert( iter != observers.end() );
  observers.erase(iter);

  if (observers.empty()) {
    holder.notifyUnobserved(diagram);
    // Note that the observed_diagram no longer exists here
  }
}


ObservedDiagrams::DiagramObserverPtr
  ObservedDiagrams::makeObserver(Diagram &diagram)
{
  auto iter = observed_diagram_map.find(&diagram);

  if (iter==observed_diagram_map.end()) {
    auto emplace_result =
      observed_diagram_map.emplace(&diagram,ObservedDiagram{diagram,*this});
    assert(/*was_inserted*/emplace_result.second);
    iter = emplace_result.first;
  }

  ObservedDiagram &observed_diagram = iter->second;
  return make_unique<DiagramObserver>(observed_diagram);
}


ObservedDiagrams::ObservedDiagram *
  ObservedDiagrams::findObservedDiagramFor(Diagram &diagram)
{
  auto iter = observed_diagram_map.find(&diagram);

  if (iter==observed_diagram_map.end()) {
    return nullptr;
  }

  ObservedDiagram &observed_diagram = iter->second;
  return &observed_diagram;
}


void ObservedDiagrams::notifyUnobserved(Diagram &diagram)
{
  auto iter = observed_diagram_map.find(&diagram);
  assert(iter!=observed_diagram_map.end());
  observed_diagram_map.erase(iter);
}
