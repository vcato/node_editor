#ifndef OBSERVEDDIAGRAM_HPP_
#define OBSERVEDDIAGRAM_HPP_


#include "diagramstate.hpp"

struct Diagram;

struct ObservedDiagram {
  Diagram &diagram;
  DiagramState diagram_state;

  struct Holder {
    virtual void notifyUnobserved(Diagram &diagram) = 0;
  };

  ObservedDiagram(Diagram &diagram_arg,Holder &holder_arg);

  void notifyDiagramStateChanged();

  struct Observer {
    ObservedDiagram &observed_diagram;
    std::function<void()> diagram_state_changed_callback;

    Observer(ObservedDiagram &observed_diagram_arg);
    ~Observer();
  };

private:
  Holder &holder;
  std::vector<Observer *> observers;

  void addObserver(Observer &observer);
  void removeObserver(Observer &observer);
};


using DiagramObserver = ObservedDiagram::Observer;
using DiagramObserverPtr = std::unique_ptr<DiagramObserver>;


#endif /* OBSERVEDDIAGRAM_HPP_ */
