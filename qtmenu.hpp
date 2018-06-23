#include <QAction>
#include <QMenuBar>
#include <functional>


extern QAction& createAction(QMenu &menu,const std::string &label);

extern void
  createAction(
    QMenu &menu,
    const std::string &label,
    const std::function<void()> &function
  );

template <typename Widget,typename Arg>
inline QMenu& createWidget(QMenuBar &parent,const Arg &arg)
{
  Widget *new_widget_ptr = new Widget(arg);
  parent.addMenu(new_widget_ptr);
  return *new_widget_ptr;
}
