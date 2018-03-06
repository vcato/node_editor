#include "mainwindow.hpp"

#include <string>
#include <algorithm>
#include "world.hpp"
#include "worldwrapper.hpp"


using std::string;
using std::vector;
using std::cerr;


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

  void
    userSelectsContextMenuItem(
      const TreePath &path,const string &operation_name
    )
  {
    vector<string> operation_names = operationNames(path);
    int index = findIndex(operation_names,operation_name);
    executeOperation(path,index);
  }

  void userChangesStringValue(const TreePath &path,const string &new_value)
  {
    stringItemValueChanged(path,new_value);
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
struct FakeSceneTree : SceneTree {
  Item root;

  void setItems(const Item &root_arg) override
  {
    root = root_arg;
  }
};
}


namespace {
struct FakeSceneWindow : SceneWindow {
  FakeSceneViewer viewer_member;
  FakeSceneTree tree_member;

  SceneViewer &viewer() override { return viewer_member; }
  SceneTree &tree() override { return tree_member; }
};
}


namespace {
struct FakeWorld : World {
  FakeSceneWindow scene_window;

  virtual SceneWindow& createSceneViewerWindow(SceneMember &)
  {
    return scene_window;
  }
};
}


static void testAddingABodyToTheScene()
{
  FakeWorld world;
  WorldWrapper world_wrapper(world);
  FakeMainWindow main_window;
  main_window.setWorldPtr(&world_wrapper);

  // User executes Add Scene in the tree editor.
  main_window.tree_editor.userSelectsContextMenuItem("Add Scene");

  // User selects Add Body on the scene.
  TreePath scene_path = makePath(world_wrapper,{"Scene1"});
  main_window.tree_editor.userSelectsContextMenuItem(scene_path,"Add Body");

  // Assert the scene window shows a body in the tree.
  assert(world.scene_window.tree_member.root.children[0].label=="Body1");
}


static void testChangingABodyName()
{
  FakeWorld world;
  WorldWrapper world_wrapper(world);
  FakeMainWindow main_window;
  main_window.setWorldPtr(&world_wrapper);

  // User executes Add Scene in the tree editor.
  main_window.tree_editor.userSelectsContextMenuItem("Add Scene");

  // User selects Add Body on the scene.
  TreePath scene_path = makePath(world_wrapper,"Scene1");
  main_window.tree_editor.userSelectsContextMenuItem(scene_path,"Add Body");

  // User changes the body name.
  TreePath body_name_path = makePath(world_wrapper,"Scene1|Body|name");
  main_window.tree_editor.userChangesStringValue(body_name_path,"Test");

  // Assert the scene window shows the new body name.
  assert(world.scene_window.tree_member.root.children[0].label=="Test");
}


int main()
{
  testAddingABodyToTheScene();
  testChangingABodyName();
}
