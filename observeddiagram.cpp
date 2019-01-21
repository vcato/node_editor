#include "observeddiagram.hpp"

#include <algorithm>

using std::cerr;


ObservedDiagram::Observer::Observer(
  ObservedDiagram &observed_diagram_arg,
  std::function<void()> diagram_changed_hook_arg
)
: observed_diagram(observed_diagram_arg),
  diagram_changed_hook(diagram_changed_hook_arg)
{
  observed_diagram.addObserver(*this);
}


ObservedDiagram::Observer::~Observer()
{
  observed_diagram.removeObserver(*this);
}


void ObservedDiagram::Observer::notifyObservedDiagramThatDiagramChanged()
{
  diagram_changed_hook();
  observed_diagram.notifyOwnerThatDiagramChanged();
}


ObservedDiagram::ObservedDiagram(Diagram &diagram_arg,Holder &holder_arg)
: diagram(diagram_arg),
  holder(holder_arg)
{
}


ObservedDiagram::~ObservedDiagram()
{
  assert(observers.empty());
}


void ObservedDiagram::notifyObserversThatDiagramStateChanged()
{
  for (Observer *observer_ptr : observers) {
    assert(observer_ptr);

    if (observer_ptr->diagram_state_changed_callback) {
      observer_ptr->diagram_state_changed_callback();
    }
  }
}


void ObservedDiagram::notifyOwnerThatDiagramChanged()
{
  holder.notifyDiagramChanged(diagram);
}


void ObservedDiagram::addObserver(Observer &observer)
{
  observers.push_back(&observer);
}


void ObservedDiagram::removeObserver(Observer &observer)
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
    holder.notifyDiagramUnobserved(diagram);
    // Note that the observed_diagram no longer exists here
  }
}
