#ifndef OBSERVEDDIAGRAMS_HPP_
#define OBSERVEDDIAGRAMS_HPP_

#include "observeddiagram.hpp"


struct ObservedDiagrams : ObservedDiagram::Holder {
  ~ObservedDiagrams();

  DiagramObserverPtr makeObserver(Diagram &diagram);

  ObservedDiagram *findObservedDiagramFor(const Diagram &diagram);

private:
  std::map<const Diagram *,ObservedDiagram> observed_diagram_map;

  void notifyUnobserved(Diagram &diagram) override;
};


#endif /* OBSERVEDDIAGRAMS_HPP_ */
