#include "qtdiagrameditorwindow.hpp"

#include <QApplication>


int main(int argc,char **argv)
{
  QApplication app(argc,argv);
  Diagram diagram;
  QtDiagramEditorWindow window;
  window.setDiagramPtr(&diagram);
  window.show();
  app.exec();
}
