#ifndef QTDIAGRAMEDITORWINDOW_HPP
#define QTDIAGRAMEDITORWINDOW_HPP

#include <QDialog>
#include "diagram.hpp"
#include "qtdiagrameditor.hpp"
#include "diagrameditorwindow.hpp"


class QtDiagramEditorWindow : public QDialog, public DiagramEditorWindow {
  public:
    QtDiagramEditorWindow();

  private:
    DiagramEditor &diagramEditor() override
    {
      assert(diagram_editor_ptr);
      return *diagram_editor_ptr;
    }

    QtDiagramEditor *diagram_editor_ptr = nullptr;

    void closeEvent(QCloseEvent *) override;
    void forceClose() override;
    bool in_force_close = false;
};

#endif /* QTDIAGRAMEDITORWINDOW_HPP */
