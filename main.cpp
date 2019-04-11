#include <QApplication>
#include <QDialog>
#include <iostream>
#include "qtmainwindow.hpp"
#include "qtsceneviewer.hpp"
#include "world.hpp"
#include "worldwrapper.hpp"
#include "qtworld.hpp"


int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  SystemFiles files;
  QtMainWindow main_window;
  main_window.setFileAccessorPtr(&files);

  QtWorld world(main_window);

  main_window.setWorldPtr(&world);
  main_window.resize(640,480);
  main_window.show();
  return app.exec();
}
