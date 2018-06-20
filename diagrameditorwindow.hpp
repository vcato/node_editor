#ifndef DIAGRAMEDITORWINDOW_HPP
#define DIAGRAMEDITORWINDOW_HPP

#include "diagrameditor.hpp"


class DiagramEditorWindow {
  public:
    void setDiagramPtr(Diagram *arg)
    {
      diagramEditor().setDiagramPtr(arg);
    }

    Diagram *diagramPtr() { return diagramEditor().diagramPtr(); }

    virtual void forceClose() = 0;

    std::function<void()> close_callback;

    std::function<void()> &diagramChangedCallback()
    {
      return diagramEditor().diagramChangedCallback();
    }

  protected:
    void notifyWindowClosing()
    {
      close_callback();
    }

  private:
    virtual DiagramEditor &diagramEditor() = 0;
};

#endif /* DIAGRAMEDITORWINDOW_HPP */
