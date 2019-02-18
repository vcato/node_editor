#ifndef OBSERVEDDIAGRAM_HPP_
#define OBSERVEDDIAGRAM_HPP_

#include "diagramstate.hpp"

struct Diagram;

struct ObservedDiagram {
  Diagram &diagram;
  DiagramState diagram_state;

  struct Holder {
    virtual void notifyDiagramUnobserved(Diagram &) = 0;
    virtual void notifyDiagramChanged(Diagram &) = 0;
  };

  ObservedDiagram(Diagram &diagram_arg,Holder &holder_arg);
  ~ObservedDiagram();

  ObservedDiagram(const ObservedDiagram &) = delete;
  void operator=(const ObservedDiagram &) = delete;

  ObservedDiagram &operator=(ObservedDiagram &&) = default;
  ObservedDiagram(ObservedDiagram &&) = default;

  void notifyObserversThatDiagramStateChanged();

  struct Observer {
    std::function<void()> diagram_state_changed_callback;
    void notifyObservedDiagramThatDiagramChanged();

    Observer(
      ObservedDiagram &observed_diagram_arg,
      std::function<void()> diagram_changed_hook
    );

    ~Observer();

    Observer(const Observer &) = delete;
    void operator=(const Observer &) = delete;

    const DiagramState &diagramState()
    {
      return observed_diagram.diagram_state;
    }

    Diagram &diagram()
    {
      return observed_diagram.diagram;
    }

    private:
      ObservedDiagram &observed_diagram;
      std::function<void()> diagram_changed_hook;
  };

private:
  Holder &holder;
  std::vector<Observer *> observers;

  void addObserver(Observer &observer);
  void removeObserver(Observer &observer);
  void notifyOwnerThatDiagramChanged();
};


using DiagramObserver = ObservedDiagram::Observer;
using DiagramObserverPtr = std::unique_ptr<DiagramObserver>;


#endif /* OBSERVEDDIAGRAM_HPP_ */
