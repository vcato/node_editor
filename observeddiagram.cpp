#include "observeddiagram.hpp"

#include <algorithm>


ObservedDiagram::Observer::Observer(
  ObservedDiagram &observed_diagram_arg
)
: observed_diagram(observed_diagram_arg)
{
  observed_diagram.addObserver(*this);
}


ObservedDiagram::Observer::~Observer()
{
  observed_diagram.removeObserver(*this);
}


ObservedDiagram::ObservedDiagram(Diagram &diagram_arg,Holder &holder_arg)
: diagram(diagram_arg),
  holder(holder_arg)
{
}


void ObservedDiagram::notifyDiagramStateChanged()
{
  for (Observer *observer_ptr : observers) {
    assert(observer_ptr);
    observer_ptr->diagram_state_changed_callback();
  }
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
    holder.notifyUnobserved(diagram);
    // Note that the observed_diagram no longer exists here
  }
}
