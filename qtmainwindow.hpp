#ifndef QTMAINWINDOW_HPP_
#define QTMAINWINDOW_HPP_

#include <QMainWindow>
#include <QMenu>
#include "wrapper.hpp"
#include "qttreeeditor.hpp"
#include "mainwindow.hpp"


class QtMainWindow : public QMainWindow, public MainWindow {
  Q_OBJECT

  public:
    QtMainWindow();

  private:
    QMenu menu{"Tools"};
    QtTreeEditor *tree_editor_ptr;

    QtTreeEditor &treeEditor();
};

#endif /* QTMAINWINDOW_HPP_ */
