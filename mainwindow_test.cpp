#include "mainwindow.hpp"

#include <string>
#include <algorithm>
#include "world.hpp"
#include "worldwrapper.hpp"
#include "streamvector.hpp"


using std::string;
using std::vector;
using std::cerr;


static int findIndex(const vector<string> &container,const string &element)
{
  auto b = container.begin();
  auto e = container.end();
  auto iter = std::find(b,e,element);

  if (iter==e) {
    cerr << "Could not find " << element << " in " << container << "\n";
  }

  assert(iter!=e);
  return iter-b;
}


namespace {
struct FakeTreeItem {
  bool is_expanded;
  string label;
  vector<FakeTreeItem> children;

  FakeTreeItem()
  : is_expanded(true)
  {
  }

  int childCount() const { return children.size(); }
  FakeTreeItem *child(int index) { return &children[index]; }

  friend FakeTreeItem& insertChildItem(FakeTreeItem &parent_item,int index)
  {
    parent_item.children.insert(
      parent_item.children.begin()+index,
      FakeTreeItem()
    );

    return parent_item.children[index];
  }

  friend void removeChildItem(FakeTreeItem &parent_item,int index)
  {
    parent_item.children.erase(parent_item.children.begin() + index);
  }

  friend void setText(FakeTreeItem &item,const string &text)
  {
    item.label = text;
  }
};
}


namespace {
struct FakeTreeEditor : TreeEditor {
  using Item = FakeTreeItem;

  Item root;

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

  void addMainTreeItem(const TreePath &new_item_path) override
  {
    Item &parent_item = itemFromPath(root,parentPath(new_item_path));
    insertChildItem(parent_item,new_item_path.back());
  }

  void replaceTreeItems(const TreePath &/*parent_path*/) override
  {
  }

  void changeEnumerationValues(const TreePath &) override
  {
  }

  void removeTreeItem(const TreePath &path) override
  {
    Item &parent_item = itemFromPath(root,parentPath(path));
    removeChildItem(parent_item,path.back());
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
  using Item = FakeTreeItem;

  Item root;

  void setItems(const ItemData &root_arg) override
  {
    root = Item();
    addBodiesTo(root,root_arg.children);
  }

  void
    insertItem(const std::vector<int> &path,const ItemData &new_item) override
  {
    Item &parent_item = itemFromPath(root,parentPath(path));
    insertBodyIn(parent_item,/*index*/path.back(),new_item);
  }

  void removeItem(const std::vector<int> &path) override
  {
    Item &parent_item = itemFromPath(root,parentPath(path));
    removeBodyFrom(parent_item,/*index*/path.back());
  }
};
}



namespace {
struct FakeSceneWindow : SceneWindow {
  FakeSceneViewer viewer_member;
  FakeSceneTree tree_member;

  SceneViewer &viewer() override { return viewer_member; }
  SceneTree &tree() override { return tree_member; }
  void setTitle(const std::string &) override { }
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
  TreePath scene_path = makePath(world_wrapper,"Scene1");
  main_window.tree_editor.userSelectsContextMenuItem(scene_path,"Add Body");

  // Assert the scene window shows a body in the tree.
  assert(world.scene_window.tree_member.root.children[0].label=="Body1");

  // User collapses the item for the first body
  world.scene_window.tree_member.root.children[0].is_expanded = false;

  // User adds another body.
  main_window.tree_editor.userSelectsContextMenuItem(scene_path,"Add Body");

  // Assert that the first body is still collapsed and the new body
  // is expanded.
  assert(world.scene_window.tree_member.root.children[0].is_expanded == false);
  assert(world.scene_window.tree_member.root.children[1].is_expanded == true);
  assert(world.scene_window.tree_member.root.children.size()==2);
}


static void testAddingABodyToABody()
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

  {
    string label_for_first_child_of_body =
      world.scene_window.tree_member.root.children[0].children[0].label;
    assert(label_for_first_child_of_body=="Position:");
  }

  // User selects Add Body on the body.
  TreePath body_path = makePath(world_wrapper,"Scene1|Body");
  main_window.tree_editor.userSelectsContextMenuItem(body_path,"Add Body");

  {
    string label_for_first_child_of_body =
      world.scene_window.tree_member.root.children[0].children[0].label;
    assert(label_for_first_child_of_body=="Position:");
  }
}


static void testRemovingAMotionPass()
{
  FakeWorld world;
  WorldWrapper world_wrapper(world);
  FakeMainWindow main_window;
  main_window.setWorldPtr(&world_wrapper);

  // User executes Add Charmapper in the tree editor.
  main_window.tree_editor.userSelectsContextMenuItem("Add Charmapper");
  TreePath charmapper_path = makePath(world_wrapper,"Charmapper1");

  // User executes Add Motion Pass on the charmapper.
  main_window.tree_editor.userSelectsContextMenuItem(
    charmapper_path,"Add Motion Pass"
  );
  TreePath motion_pass_path = makePath(world_wrapper,"Charmapper1|Motion Pass");

  // Check that the charmapper has one child.
  assert(world.charmapperMember(0).charmapper.nPasses()==1);

  // User executes Remove on the motion pass.
  main_window.tree_editor.userSelectsContextMenuItem(motion_pass_path,"Remove");

  // Check that the charmapper has no children.
  assert(world.charmapperMember(0).charmapper.nPasses()==0);

  // Check that the item was removed from the tree editor.
  assert(main_window.tree_editor.root.children[0].childCount()==0);
}


static void testChangingABodyName()
{
  FakeWorld world;
  WorldWrapper world_wrapper(world);
  FakeMainWindow main_window;
  main_window.setWorldPtr(&world_wrapper);

  // User executes Add Scene in the tree editor.
  main_window.tree_editor.userSelectsContextMenuItem("Add Scene");
  TreePath scene_path = makePath(world_wrapper,"Scene1");

  // User selects Add Body on the scene.
  main_window.tree_editor.userSelectsContextMenuItem(scene_path,"Add Body");

  // User changes the body name.
  TreePath body_name_path = makePath(world_wrapper,"Scene1|Body|name");
  main_window.tree_editor.userChangesStringValue(body_name_path,"Test");

  // Assert the scene window shows the new body name.
  assert(world.scene_window.tree_member.root.children[0].label=="Test");
}


static void testRemovingABody()
{
  FakeWorld world;
  WorldWrapper world_wrapper(world);
  FakeMainWindow main_window;
  main_window.setWorldPtr(&world_wrapper);

  // User executes Add Scene in the tree editor.
  main_window.tree_editor.userSelectsContextMenuItem("Add Scene");
  TreePath scene_path = makePath(world_wrapper,"Scene1");

  // User selects Add Body on the scene.
  main_window.tree_editor.userSelectsContextMenuItem(scene_path,"Add Body");
  TreePath body_path = makePath(world_wrapper,"Scene1|Body");
  assert(world.sceneMember(0).scene.bodies().size()==1);
  assert(world.scene_window.tree_member.root.children.size()==1);

  // User selects Remove on the body.
  main_window.tree_editor.userSelectsContextMenuItem(body_path,"Remove");

  // Assert that the body was removed from the scene.
  assert(world.sceneMember(0).scene.bodies().size()==0);

  // Assert the body was removed from the tree editor.
  assert(main_window.tree_editor.root.children[0].children.size()==0);

  // Assert the body was removed from the scene view.
  assert(world.scene_window.tree_member.root.children.size()==0);
}


int main()
{
  testAddingABodyToTheScene();
  testAddingABodyToABody();
  testRemovingAMotionPass();
  testChangingABodyName();
  testRemovingABody();
}
