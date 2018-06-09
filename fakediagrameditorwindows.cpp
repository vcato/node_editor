#include "fakediagrameditorwindows.hpp"

#include "removefrom.hpp"


void FakeDiagramEditorWindow::userClosesWindow()
{
  notifyWindowClosing();
  removeFrom(all_windows,this);
}


void FakeDiagramEditorWindow::forceClose()
{
  removeFrom(all_windows,this);
}
