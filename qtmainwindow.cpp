#include "qtmainwindow.hpp"

#include <cassert>
#include <iostream>
#include <QMenuBar>


using std::cerr;


QtMainWindow::QtMainWindow()
: diagram_editor(diagram)
{
  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  menu_bar_ptr->addMenu(&menu);

  setCentralWidget(&diagram_editor);
}
