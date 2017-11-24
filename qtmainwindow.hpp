#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QTreeWidget>
#include "qtdiagrameditor.hpp"


class QtMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    QtMainWindow();

  private:
    QMenu menu{"Tools"};
    Diagram diagram;
    QWidget widget;
};
