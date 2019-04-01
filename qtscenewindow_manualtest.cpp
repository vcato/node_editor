#include "qtscenewindow.hpp"

#include <QApplication>
#include "scene.hpp"
#include "streamvector.hpp"


using std::cerr;


namespace {
struct ListenerStub : SceneListener {
  QtSceneWindow &window;
  Scene &scene;

  ListenerStub(QtSceneWindow &window_arg,Scene &scene_arg)
  : window(window_arg),
    scene(scene_arg)
  {
  }

  void
    frameVariablesChanged(
      int frame_index,
      std::vector<int> &variable_indices
    ) override
  {
    cerr << "frameVariablesChanged(\n";
    cerr << "  frame_index=" << frame_index << ",\n";
    cerr << "  variable_indices=" << variable_indices << "\n";
    cerr << ")\n";
    scene.displayFrame() = scene.backgroundFrame();
    window.notifySceneChanged();
  }
};
}


int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  Scene scene;
  Scene::Body &body1 = scene.addBody();
  scene.addChildBodyTo(body1);
  scene.addBody();
  QtSceneWindow scene_window(nullptr);
  ListenerStub listener{scene_window,scene};
  scene_window.setScenePtr(&scene,&listener,"Scene Window");
  scene_window.show();
  return app.exec();
}
