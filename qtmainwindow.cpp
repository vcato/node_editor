#include "qtmainwindow.hpp"

#include <cassert>
#include <iostream>
#include <QMenuBar>


using std::cerr;


QtMainWindow::QtMainWindow()
{
  QAction *action_ptr = menu.addAction("Add Test Node");
  connect(action_ptr,SIGNAL(triggered()),SLOT(addTestNodeClicked()));

  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  menu_bar_ptr->addMenu(&menu);

  setCentralWidget(&diagram_editor);
}


void QtMainWindow::addTestNodeClicked()
{
  diagram_editor.addTestNode();
}
