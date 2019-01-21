#ifndef OBSERVEDDIAGRAMS_HPP_
#define OBSERVEDDIAGRAMS_HPP_

#include "observeddiagram.hpp"


struct ObservedDiagrams : ObservedDiagram::Holder {
  using DiagramChangedCallback = std::function<void(const Diagram &)>;
  ObservedDiagrams(DiagramChangedCallback = [](const Diagram &){});

  ~ObservedDiagrams();

  DiagramObserverPtr
    makeObserver(
      Diagram &diagram,
      std::function<void()> diagram_changed_hook = [](){}
    );

  ObservedDiagram *findObservedDiagramFor(const Diagram &diagram);

private:
  std::map<const Diagram *,ObservedDiagram> observed_diagram_map;
  DiagramChangedCallback diagram_changed_callback;

  void notifyDiagramUnobserved(Diagram &diagram) override;
  void notifyDiagramChanged(Diagram &) override;
};


#endif /* OBSERVEDDIAGRAMS_HPP_ */
