#include "qtmenu.hpp"

#include <QMenu>


using std::string;


QAction& createAction(QMenu &menu,const string &label)
{
  QAction *add_pass_action_ptr = new QAction(QString::fromStdString(label),0);
  menu.addAction(add_pass_action_ptr);
  return *add_pass_action_ptr;
}
