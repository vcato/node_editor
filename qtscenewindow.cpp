#include "qtscenewindow.hpp"

#include <QBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
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
  {
    assert(tree.header());
    tree.header()->close();
  }
  QtSceneViewer &viewer = createWidget<QtSceneViewer>(layout);
  viewer_ptr = &viewer;
}


static void
  addBodiesTo(QTreeWidgetItem &parent_item,const Scene::Bodies &bodies)
{
  for (auto &body : bodies) {
    QTreeWidgetItem &item1 = createChildItem(parent_item);
    item1.setText(/*column*/0,QString::fromStdString(body.name));
    addBodiesTo(item1,body.children);
  }
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

  assert(tree.invisibleRootItem());
  addBodiesTo(*tree.invisibleRootItem(),arg->bodies());
}


QtSceneViewer &QtSceneWindow::viewer()
{
  assert(viewer_ptr);
  return *viewer_ptr;
}
