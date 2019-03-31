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
  WorldWrapper world_wrapper(world);
  main_window.setWorldPtr(&world_wrapper);
#if 0
  struct World::Client {
    virtual void sceneVariableChanged(int scene_index,int variable_index)
    {
      TreePath variable_path =
        world_wrapper.makeSceneVariablePath(scene_index,variable_index);
      main_window.itemValueChanged(variable_path);
    }
  };

  world.setClientPtr(&client);
#endif
  main_window.resize(640,480);
  main_window.show();
  return app.exec();
}
