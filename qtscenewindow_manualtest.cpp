#include "qtscenewindow.hpp"

#include <QApplication>
#include "scene.hpp"


int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  Scene scene;
  scene.addBody();
  QtSceneWindow scene_window(nullptr);
  scene_window.setScenePtr(&scene);
  scene_window.show();
  return app.exec();
}
