#include "diagramstate.hpp"

struct Diagram;

struct ObservedDiagrams {
  struct ObservedDiagram {
    Diagram &diagram;
    DiagramState diagram_state;

    ObservedDiagram(Diagram &diagram_arg,ObservedDiagrams &holder_arg);

    void notifyDiagramStateChanged();

    struct Observer {
      ObservedDiagram &observed_diagram;
      std::function<void()> diagram_state_changed_callback;

      Observer(ObservedDiagram &observed_diagram_arg);
      ~Observer();
    };

  private:
    ObservedDiagrams &holder;
    std::vector<Observer *> observers;

    void addObserver(Observer &observer);
    void removeObserver(Observer &observer);
  };

  using DiagramObserver = ObservedDiagram::Observer;
  using DiagramObserverPtr = std::unique_ptr<DiagramObserver>;

  DiagramObserverPtr makeObserver(Diagram &diagram);

  ObservedDiagram *findObservedDiagramFor(Diagram &diagram);

private:
  std::map<Diagram *,ObservedDiagram> observed_diagram_map;

  void notifyUnobserved(Diagram &diagram);
};
