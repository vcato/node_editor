#include "qtscenewindow.hpp"

#include <QBoxLayout>
#include <QTreeWidget>
#include "qtwidget.hpp"
#include "qtsceneviewer.hpp"
#include "qttreewidgetitem.hpp"


QtSceneWindow::QtSceneWindow(QWidget *parent_widget_ptr)
: QDialog(parent_widget_ptr),
  tree_ptr(0)
{
  QBoxLayout &layout = createLayout<QHBoxLayout>(*this);
  QTreeWidget &tree = createWidget<QTreeWidget>(layout);
  tree_ptr = &tree;
  QtSceneViewer &viewer = createWidget<QtSceneViewer>(layout);
  viewer_ptr = &viewer;
}


void QtSceneWindow::setScenePtr(Scene *arg)
{
  viewer().setScenePtr(arg);
  assert(tree_ptr);
  QTreeWidget &tree = *tree_ptr;
  tree.clear();

  if (!arg) {
    return;
  }

  for (auto &body : arg->bodies()) {
    assert(tree.invisibleRootItem());
    QTreeWidgetItem &item1 = createChildItem(*tree.invisibleRootItem());
    item1.setText(/*column*/0,QString::fromStdString(body.name));
  }
}


QtSceneViewer &QtSceneWindow::viewer()
{
  assert(viewer_ptr);
  return *viewer_ptr;
}
