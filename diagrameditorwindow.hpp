#ifndef DIAGRAMEDITORWINDOW_HPP
#define DIAGRAMEDITORWINDOW_HPP

#include "diagrameditor.hpp"


class DiagramEditorWindow {
  public:
    void setDiagramPtr(Diagram *arg);
    Diagram *diagramPtr() { return diagramEditor().diagramPtr(); }

    std::function<void()> close_callback;
    std::function<void()> &diagramChangedCallback();
    virtual void forceClose() = 0;

  protected:
    void notifyWindowClosing();

  private:
    virtual DiagramEditor &diagramEditor() = 0;
};

#endif /* DIAGRAMEDITORWINDOW_HPP */
