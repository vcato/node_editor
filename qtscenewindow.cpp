#include "qtscenewindow.hpp"

#include <QBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include "qtwidget.hpp"
#include "qtsceneviewer.hpp"
#include "qttreewidgetitem.hpp"
#include "qtscenetree.hpp"
#include "qtlayout.hpp"


QtSceneWindow::QtSceneWindow(QWidget *parent_widget_ptr)
: QDialog(parent_widget_ptr),
  tree_ptr(0)
{
  QBoxLayout &layout = createLayout<QHBoxLayout>(*this);
  QtSceneTree &tree = createWidget<QtSceneTree>(layout);
  tree_ptr = &tree;
  QtSceneViewer &viewer = createWidget<QtSceneViewer>(layout);
  viewer_ptr = &viewer;
}


SceneViewer &QtSceneWindow::viewer()
{
  assert(viewer_ptr);
  return *viewer_ptr;
}


SceneTree &QtSceneWindow::tree()
{
  assert(tree_ptr);
  return *tree_ptr;
}


void QtSceneWindow::setTitle(const std::string &arg)
{
  QDialog::setWindowTitle(QString::fromStdString(arg));
}
