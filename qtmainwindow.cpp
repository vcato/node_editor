#include "qtmainwindow.hpp"

#include <iostream>
#include <QMenuBar>
#include <QFileDialog>
#include <QErrorMessage>
#include <QBoxLayout>
#include <QSplitter>
#include "qtmenu.hpp"
#include "qtwidget.hpp"
#include "qttreeeditor.hpp"
#include "qtmenubar.hpp"
#include "qtsplitter.hpp"


using std::cerr;
using std::string;


template <typename Layout>
static Layout& createLayout(QBoxLayout &parent_layout)
{
  Layout *layout_ptr = new Layout;
  parent_layout.addLayout(layout_ptr);
  return *layout_ptr;
}


template <typename Widget>
static Widget& createCentralWidget(QMainWindow &parent)
{
  Widget *widget_ptr = new Widget;
  parent.setCentralWidget(widget_ptr);
  return *widget_ptr;
}


QtMainWindow::QtMainWindow()
: tree_editor_ptr(0)
{
  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  QMenu &tools_menu = createWidget<QMenu>(*menu_bar_ptr,"Tools");
  createAction(tools_menu,"Open Project...",[this](){_openProjectPressed();});
  createAction(tools_menu,"Save Project...",[this](){_saveProjectPressed();});

  QSplitter &splitter = createCentralWidget<QSplitter>(*this);

  QtTreeEditor &tree_editor = createWidget<QtTreeEditor>(splitter);
  tree_editor_ptr = &tree_editor;
}


TreeEditor &QtMainWindow::treeEditor()
{
  assert(tree_editor_ptr);
  return *tree_editor_ptr;
}


Optional<string> QtMainWindow::_askForSavePath()
{
  QFileDialog file_dialog;
  QString result =
    file_dialog.getSaveFileName(this,"Save Project","projects/project.dat");

  if (result=="") {
    return {};
  }

  return result.toStdString();
}


string QtMainWindow::_askForOpenPath()
{
  QFileDialog file_dialog;
  QString result =
    file_dialog.getOpenFileName(this,"Open Project","projects/project.dat");
  return result.toStdString();
}


void QtMainWindow::_showError(const std::string &message)
{
  QErrorMessage *error_message_ptr = new QErrorMessage(this);
  error_message_ptr->showMessage(QString::fromStdString(message));
}
