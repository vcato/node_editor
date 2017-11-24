#include "qtmainwindow.hpp"

#include <cassert>
#include <iostream>
#include <QMenuBar>
#include <QBoxLayout>


using std::cerr;
using std::string;


template <typename Layout>
static Layout& setLayout(QWidget &widget)
{
  Layout *layout_ptr = new Layout;
  widget.setLayout(layout_ptr);
  return *layout_ptr;
}


static QTreeWidget& addTreeWidgetTo(QLayout &layout)
{
  QTreeWidget *tree_widget_ptr = new QTreeWidget;

  layout.addWidget(tree_widget_ptr);
  return *tree_widget_ptr;
}


static void addDiagramEditorTo(QLayout &layout,Diagram &diagram)
{
  QtDiagramEditor *diagram_editor_ptr = new QtDiagramEditor(diagram);
  layout.addWidget(diagram_editor_ptr);
}


static void addItemTo(QTreeWidget &tree_widget,const string &label)
{
  QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
  item_ptr->setText(/*column*/0,QString::fromStdString(label));
  tree_widget.addTopLevelItem(item_ptr);
}


QtMainWindow::QtMainWindow()
{
  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  menu_bar_ptr->addMenu(&menu);

  QHBoxLayout &layout = ::setLayout<QHBoxLayout>(widget);

  QTreeWidget &tree_widget = addTreeWidgetTo(layout);
  addItemTo(tree_widget,"charmapper");
  addDiagramEditorTo(layout,diagram);
  setCentralWidget(&widget);
}
