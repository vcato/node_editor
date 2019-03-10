#include "diagrameditorwindow.hpp"

#include <iostream>

using std::cerr;


void DiagramEditorWindow::notifyWindowClosing()
{
  if (!close_callback) {
    return;
  }

  close_callback();
}


void DiagramEditorWindow::setDiagramObserver(DiagramObserverPtr arg)
{
  diagramEditor().setDiagramObserver(std::move(arg));
}
