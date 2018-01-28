#ifndef QTWIDGET_HPP_
#define QTWIDGET_HPP_

#include <cassert>
#include <QLayout>
#include <QSplitter>

template <typename Layout>
Layout& createLayout(QWidget &widget)
{
  Layout *layout_ptr = new Layout;
  widget.setLayout(layout_ptr);
  return *layout_ptr;
}


template <typename Widget>
Widget& createWidget(QLayout &layout)
{
  Widget *widget_ptr = new Widget;
  layout.addWidget(widget_ptr);
  assert(widget_ptr);
  return *widget_ptr;
}


template <typename Widget>
Widget& createWidget(QWidget &parent_widget)
{
  Widget *widget_ptr = new Widget(&parent_widget);
  assert(widget_ptr);
  return *widget_ptr;
}



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


#endif /* QTWIDGET_HPP_ */
