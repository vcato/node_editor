#ifndef QTDIAGRAMEDITORWINDOW_HPP
#define QTDIAGRAMEDITORWINDOW_HPP

#include <QDialog>
#include "diagrameditorwindow.hpp"


class QtDiagramEditor;


class QtDiagramEditorWindow : public QDialog, public DiagramEditorWindow {
  public:
    QtDiagramEditorWindow();
    ~QtDiagramEditorWindow();
    DiagramEditor &diagramEditor() override;

  private:

    QtDiagramEditor *diagram_editor_ptr = nullptr;

    void closeEvent(QCloseEvent *) override;
    void forceClose() override;
    bool in_force_close = false;
};

#endif /* QTDIAGRAMEDITORWINDOW_HPP */
