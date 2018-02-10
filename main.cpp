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
  QtMainWindow main_window;
  QtWorld world(main_window);
  WorldWrapper world_wrapper(world);
  main_window.setWorldPtr(&world_wrapper);
  main_window.show();
  return app.exec();
}
