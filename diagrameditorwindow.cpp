#include "diagrameditorwindow.hpp"


void DiagramEditorWindow::notifyWindowClosing()
{
  if (!close_callback) return;

  close_callback();
}


std::function<void()> &DiagramEditorWindow::diagramChangedCallback()
{
  return diagramEditor().diagramChangedCallback();
}


void DiagramEditorWindow::setDiagramPtr(Diagram *arg)
{
  diagramEditor().setDiagramPtr(arg);
}


void DiagramEditorWindow::setDiagramObserver(DiagramObserverPtr arg)
{
  diagramEditor().setDiagramObserver(std::move(arg));
}
