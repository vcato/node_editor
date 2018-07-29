#include <QSplitter>


template <typename Widget>
Widget& createWidget(QSplitter &splitter)
{
  Widget *widget_ptr = new Widget;
  splitter.addWidget(widget_ptr);
  assert(widget_ptr);
  return *widget_ptr;
}


template <typename Widget>
Widget& createWidget(QSplitter &splitter,int stretch)
{
  int index = splitter.count();
  Widget *widget_ptr = new Widget;
  splitter.addWidget(widget_ptr);
  splitter.setStretchFactor(index,stretch);
  return *widget_ptr;
}
