#include "mainwindow.hpp"

#include <string>
#include <algorithm>
#include "world.hpp"
#include "worldwrapper.hpp"


using std::string;
using std::vector;


static int findIndex(const vector<string> &container,const string &element)
{
  auto b = container.begin();
  auto e = container.end();
  auto iter = std::find(b,e,element);
  assert(iter!=e);
  return iter-b;
}


namespace {
struct FakeTreeEditor : TreeEditor {
  void userSelectsContextMenuItem(const string &operation_name)
  {
    TreePath path = {};
    vector<string> operation_names = operationNames(path);
    int index = findIndex(operation_names,operation_name);
    executeOperation(path,index);
  }

  void addTreeItem(const TreePath & /*new_item_path*/) override
  {
  }

  void replaceTreeItems(const TreePath &/*parent_path*/) override
  {
  }

  void changeEnumerationValues(const TreePath &) override
  {
  }
};
}


namespace {
struct FakeMainWindow : MainWindow {
  FakeTreeEditor &treeEditor() override { return tree_editor; }

  FakeTreeEditor tree_editor;
};
}


namespace {
struct FakeSceneViewer : SceneViewer {
  virtual void redrawScene()
  {
  }
};
}


namespace {
struct FakeWorld : World {
  FakeSceneViewer scene_viewer;

  virtual SceneViewer& createSceneViewerWindow(SceneMember &)
  {
    return scene_viewer;
  }
};
}


#if 0
static void testAddingABodyToTheScene()
{
  FakeWorld world;
  WorldWrapper world_wrapper(world);
  FakeMainWindow main_window;
  main_window.setWorldPtr(&world_wrapper);

  // User executes Add Scene in the tree editor.
  main_window.tree_editor.userSelectsContextMenuItem("Add Scene");

  // Use selects Add Body on the scene.
  assert(false); // not implemented

  // Assert the scene window shows a body in the tree.
  assert(false); // not implemented
}
#endif


int main()
{
#if 0
  testAddingABodyToTheScene();
#endif
}
