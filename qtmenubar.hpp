#include <QMenu>
#include <QMenuBar>


template <typename Widget,typename Arg>
inline Widget& createWidget(QMenuBar &parent,const Arg &arg)
{
  Widget *new_widget_ptr = new Widget(arg);
  parent.addMenu(new_widget_ptr);
  return *new_widget_ptr;
}
