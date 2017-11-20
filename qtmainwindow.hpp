#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include "qtdiagrameditor.hpp"


class QtMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    QtMainWindow();

  private:
    QMenu menu{"Tools"};
    QtDiagramEditor diagram_editor;
};
