#ifndef QTMAINWINDOW_HPP_
#define QTMAINWINDOW_HPP_

#include <QMainWindow>
#include "wrapper.hpp"
#include "mainwindow.hpp"


class QtTreeEditor;


class QtMainWindow : public QMainWindow, public MainWindow {
  Q_OBJECT

  public:
    QtMainWindow();

  private:
    QtTreeEditor *tree_editor_ptr;

    TreeEditor &treeEditor() override;
    Optional<std::string> _askForSavePath() override;
    std::string _askForOpenPath() override;
    void _showError(const std::string &message) override;
};

#endif /* QTMAINWINDOW_HPP_ */
