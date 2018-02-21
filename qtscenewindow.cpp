#include "qtscenewindow.hpp"

#include <QBoxLayout>
#include "qtwidget.hpp"
#include "qtsceneviewer.hpp"


QtSceneWindow::QtSceneWindow(QWidget *parent_widget_ptr)
: QDialog(parent_widget_ptr)
{
  QBoxLayout &layout = createLayout<QVBoxLayout>(*this);
  QtSceneViewer &viewer = createWidget<QtSceneViewer>(layout);
  viewer_ptr = &viewer;
}


void QtSceneWindow::setScenePtr(Scene *arg)
{
  viewer().setScenePtr(arg);
}


QtSceneViewer &QtSceneWindow::viewer()
{
  assert(viewer_ptr);
  return *viewer_ptr;
}
