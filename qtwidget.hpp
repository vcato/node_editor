#ifndef QTWIDGET_HPP_
#define QTWIDGET_HPP_

#include <cassert>
#include <QLayout>

template <typename Layout>
inline Layout& createLayout(QWidget &widget)
{
  Layout *layout_ptr = new Layout;
  widget.setLayout(layout_ptr);
  return *layout_ptr;
}


template <typename Widget>
static Widget& createWidget(QLayout &layout)
{
  Widget *widget_ptr = new Widget;
  layout.addWidget(widget_ptr);
  assert(widget_ptr);
  return *widget_ptr;
}


#endif /* QTWIDGET_HPP_ */
