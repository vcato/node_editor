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
  struct Item {
    bool is_expanded;
    string label;
    vector<Item> children;

    Item()
    : is_expanded(true)
    {
    }

    int childCount() const { return children.size(); }
    Item *child(int index) { return &children[index]; }

    friend Item& insertChildItem(Item &parent_item,int index)
    {
      parent_item.children.insert(
        parent_item.children.begin()+index,
        Item()
      );

      return parent_item.children[index];
    }

    friend void setText(Item &item,const string &text)
    {
      item.label = text;
    }
  };

  Item root;

  void setItems(const ItemData &root_arg) override
  {
    root = Item();
    addBodiesTo(root,root_arg.children);
  }

  void
    insertItem(const std::vector<int> &path,const ItemData &new_item) override
  {
    vector<int> parent_path = path;
    parent_path.pop_back();
    Item &parent_item = itemFromPath(root,parent_path);
    insertBodyIn(parent_item,/*index*/path.back(),new_item);
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
  TreePath scene_path = makePath(world_wrapper,{"Scene1"});
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


int main()
{
  testAddingABodyToTheScene();
  testChangingABodyName();
}
