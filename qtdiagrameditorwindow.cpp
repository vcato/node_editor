#include "qtdiagrameditorwindow.hpp"

#include <iostream>
#include "qtwidget.hpp"


using std::cerr;


QtDiagramEditorWindow::QtDiagramEditorWindow()
{
  auto &layout = createLayout<QVBoxLayout>(*this);
  diagram_editor_ptr = &createWidget<QtDiagramEditor>(layout);
}


void QtDiagramEditorWindow::closeEvent(QCloseEvent *)
{
  if (in_force_close) return;

  notifyWindowClosing();
}


void QtDiagramEditorWindow::forceClose()
{
  in_force_close = true;
  QDialog::close();
  in_force_close = false;
}
