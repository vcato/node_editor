#include "qtmainwindow.hpp"

#include <QMenuBar>


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
  menu_bar_ptr->addMenu(&menu);

  QSplitter &splitter = createCentralWidget<QSplitter>(*this);

  QtTreeEditor &tree_editor = createWidget<QtTreeEditor>(splitter);
  tree_editor_ptr = &tree_editor;
}


QtTreeEditor &QtMainWindow::treeEditor()
{
  assert(tree_editor_ptr);
  return *tree_editor_ptr;
}
