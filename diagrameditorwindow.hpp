#ifndef DIAGRAMEDITORWINDOW_HPP
#define DIAGRAMEDITORWINDOW_HPP

#include "diagrameditor.hpp"


class DiagramEditorWindow {
  public:
    void setDiagramObserver(DiagramObserverPtr);

    Diagram *diagramPtr() { return diagramEditor().diagramPtr(); }

    std::function<void()> &closeCallback() { return close_callback; }
    virtual void forceClose() = 0;

  protected:
    void notifyWindowClosing();

  private:
    virtual DiagramEditor &diagramEditor() = 0;
    std::function<void()> close_callback;
};

#endif /* DIAGRAMEDITORWINDOW_HPP */
