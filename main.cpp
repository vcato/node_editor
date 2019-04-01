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

#if 0
  world.scene_frame_variables_changed_function = [&](
    int scene_member_index,
    int frame_index,
    const std::vector<int> &variable_indices
  )
  {
    for (int variable_index : variable_indices) {
      TreePath variable_path =
        world_wrapper.makeSceneVariablePath(
          scene_member_index,
          frame_index,
          variable_index
        );
      main_window.itemValueChanged(variable_path);
    }
  };
#else
  // This logic should bein the main window
#endif

  main_window.setWorldPtr(&world_wrapper);
  main_window.resize(640,480);
  main_window.show();
  return app.exec();
}
