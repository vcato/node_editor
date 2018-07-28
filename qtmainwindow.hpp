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
    QtTreeEditor *tree_editor_ptr;

    QtTreeEditor &treeEditor() override;
    std::string _askForSavePath() override;
    std::string _askForOpenPath() override;
    void _showError(const std::string &message) override;
};

#endif /* QTMAINWINDOW_HPP_ */
