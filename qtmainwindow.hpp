#include <QMainWindow>
#include <QMenu>
#include "wrapper.hpp"
#include "qttreeeditor.hpp"


class QtMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    QtMainWindow();
    void setWorldPtr(Wrapper *);

  private:
    QMenu menu{"Tools"};
    QtTreeEditor *tree_editor_ptr;
    QtDiagramEditor *diagram_editor_ptr;
};
