#ifndef QTDIAGRAMEDITORWINDOW_HPP
#define QTDIAGRAMEDITORWINDOW_HPP

#include <QDialog>
#include "diagrameditorwindow.hpp"


class QtDiagramEditor;


class QtDiagramEditorWindow : public QDialog, public DiagramEditorWindow {
  public:
    QtDiagramEditorWindow();

  private:
    DiagramEditor &diagramEditor() override;

    QtDiagramEditor *diagram_editor_ptr = nullptr;

    void closeEvent(QCloseEvent *) override;
    void forceClose() override;
    bool in_force_close = false;
};

#endif /* QTDIAGRAMEDITORWINDOW_HPP */
