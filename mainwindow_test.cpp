#include "mainwindow.hpp"

#include <string>
#include <algorithm>
#include "world.hpp"
#include "worldwrapper.hpp"
#include "streamvector.hpp"
#include "wrapperutil.hpp"
#include "fakediagrameditorwindows.hpp"
#include "optional.hpp"


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
  const FakeTreeItem *child(int index) const { return &children[index]; }

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

  void
    userSelectsContextMenuItem(
      const string &path_string,
      const string &operation_name
    )
  {
    TreePath path = makePath(world(),path_string);
    userSelectsContextMenuItem(path,operation_name);
  }


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
    if (operation_name=="Edit Diagram...") {
      openDiagramEditor(path);
    }
    else {
      executeOperation(path,operationIndex2(world(),path,operation_name));
    }
  }

  void userChangesStringValue(const TreePath &path,const string &new_value)
  {
    stringItemValueChanged(path,new_value);
  }

  void userChangesNumberValue(const TreePath &path,int new_value)
  {
    numberItemValueChanged(path,new_value);
  }

  void userChangesNumberValue(const string &path_string,int new_value)
  {
    userChangesNumberValue(makePath(world(),path_string),new_value);
  }

  int itemChildCount(const TreePath &parent_path) const override
  {
    return itemFromPath(root,parent_path).children.size();
  }

  virtual void
    setEnumerationValues(
      const TreePath &,
      const std::vector<std::string> & /*items*/
    )
  {
    assert(false);
  }

  void removeTreeItem(const TreePath &path) override
  {
    Item &parent_item = itemFromPath(root,parentPath(path));
    removeChildItem(parent_item,path.back());
  }

  virtual void removeChildItems(const TreePath &/*path*/)
  {
    assert(false);
  }

  virtual DiagramEditorWindow& createDiagramEditor()
  {
    return diagram_editor_windows.create();
  }

  void
    createVoidItem(
      const TreePath &parent_path,
      const std::string & /*label*/
    ) override
  {
    Item &parent_item = itemFromPath(root,parent_path);
    insertChildItem(parent_item,parent_item.children.size());
  }

  void
    createNumericItem(
      const TreePath & parent_path,
      const std::string & /*label*/,
      const NumericValue /*value*/
    ) override
  {
    Item &parent_item = itemFromPath(root,parent_path);
    insertChildItem(parent_item,parent_item.children.size());
  }

  void
    createEnumerationItem(
      const TreePath &parent_path,
      const std::string &/*label*/,
      const std::vector<std::string> &/*options*/,
      int /*value*/
    ) override
  {
    Item &parent_item = itemFromPath(root,parent_path);
    insertChildItem(parent_item,parent_item.children.size());
  }

  void
    createStringItem(
      const TreePath &parent_path,
      const std::string &/*label*/,
      const std::string &/*value*/
    ) override
  {
    Item &parent_item = itemFromPath(root,parent_path);
    insertChildItem(parent_item,parent_item.children.size());
  }

  FakeDiagramEditorWindows diagram_editor_windows;
};
}


namespace {
struct FakeMainWindow : MainWindow {
  FakeTreeEditor &treeEditor() override { return tree_editor; }

  std::string _askForSavePath() override
  {
    return *maybe_save_path;
  }

  std::string _askForOpenPath() override
  {
    return *maybe_open_path;
  }

  void _showError(const std::string &/*message*/) override
  {
  }

  FakeTreeEditor tree_editor;
  Optional<string> maybe_save_path;
  Optional<string> maybe_open_path;

  void userPressesSaveProject(const string &chosen_path)
  {
    maybe_save_path = chosen_path;
    _saveProjectPressed();
    maybe_save_path.reset();
  }

  void userPressesOpenProject(const string &chosen_path)
  {
    maybe_open_path = chosen_path;
    _openProjectPressed();
    maybe_open_path.reset();
  }
};
}


namespace {
struct FakeSceneViewer : SceneViewer {
  int redraw_count = 0;

  virtual void redrawScene()
  {
    ++redraw_count;
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


namespace {
struct Tester {
  FakeWorld world;
  WorldWrapper world_wrapper{world};
  FakeMainWindow main_window;

  Tester()
  {
    main_window.setWorldPtr(&world_wrapper);
  }
};
}


static bool
  containsBody(const FakeSceneTree &scene_tree,const string &body_name)
{
  // This is over-specific.  It can be changed later if needed.
  return scene_tree.root.children[0].label == body_name;
}

static void testAddingABodyToTheScene()
{
  Tester tester;

  FakeMainWindow &main_window = tester.main_window;
  FakeWorld &world = tester.world;
  FakeTreeEditor &tree_editor = main_window.tree_editor;
  FakeSceneTree &scene_tree = world.scene_window.tree_member;
  FakeSceneViewer &scene_viewer = world.scene_window.viewer_member;

  // User executes Add Scene in the tree editor.
  tree_editor.userSelectsContextMenuItem("Add Scene");

  scene_viewer.redraw_count = 0;

  // User selects Add Body on the scene.
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");

  assert(containsBody(scene_tree,"Body1"));
  assert(scene_viewer.redraw_count==1);

  // User collapses the item for the first body
  scene_tree.root.children[0].is_expanded = false;

  // User adds another body.
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");

  // Assert that the first body is still collapsed and the new body
  // is expanded.
  assert(scene_tree.root.children[0].is_expanded == false);
  assert(scene_tree.root.children[1].is_expanded == true);
  assert(scene_tree.root.children.size()==2);
}


static void testAddingABodyToABody()
{
  Tester tester;

  FakeWorld &world = tester.world;
  FakeMainWindow &main_window = tester.main_window;
  FakeTreeEditor &tree_editor = main_window.tree_editor;
  FakeSceneWindow &scene_window = world.scene_window;
  FakeSceneTree &scene_tree = scene_window.tree_member;

  // User executes Add Scene in the tree editor.
  tree_editor.userSelectsContextMenuItem("Add Scene");

  // User selects Add Body on the scene.
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");

  {
    string label_for_first_child_of_body =
      scene_tree.root.children[0].children[0].label;
    assert(label_for_first_child_of_body=="Position:");
  }

  // User selects Add Body on the body.
  tree_editor.userSelectsContextMenuItem("Scene1|Body","Add Body");

  {
    string label_for_first_child_of_body =
      scene_tree.root.children[0].children[0].label;
    assert(label_for_first_child_of_body=="Position:");
  }
}


static void testRemovingAMotionPass()
{
  Tester tester;

  FakeWorld &world = tester.world;
  WorldWrapper &world_wrapper = tester.world_wrapper;
  FakeMainWindow &main_window = tester.main_window;
  FakeTreeEditor &tree_editor = main_window.tree_editor;

  // User executes Add Charmapper in the tree editor.
  tree_editor.userSelectsContextMenuItem("Add Charmapper");

  // User executes Add Motion Pass on the charmapper.
  tree_editor.userSelectsContextMenuItem("Charmapper1","Add Motion Pass");
  TreePath motion_pass_path = makePath(world_wrapper,"Charmapper1|Motion Pass");

  // Check that the charmapper has one child.
  assert(world.charmapperMember(0).charmapper.nPasses()==1);

  // User executes Remove on the motion pass.
  tree_editor.userSelectsContextMenuItem(motion_pass_path,"Remove");

  // Check that the charmapper has no children.
  assert(world.charmapperMember(0).charmapper.nPasses()==0);

  // Check that the item was removed from the tree editor.
  assert(tree_editor.root.children[0].childCount()==0);
}


static void testChangingABodyName()
{
  Tester tester;
  FakeWorld &world = tester.world;
  WorldWrapper &world_wrapper = tester.world_wrapper;
  FakeMainWindow &main_window = tester.main_window;
  FakeTreeEditor &tree_editor = main_window.tree_editor;

  // User executes Add Scene in the tree editor.
  tree_editor.userSelectsContextMenuItem("Add Scene");

  // User selects Add Body on the scene.
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");

  // User changes the body name.
  TreePath body_name_path = makePath(world_wrapper,"Scene1|Body|name");
  tree_editor.userChangesStringValue(body_name_path,"Test");

  // Assert the scene window shows the new body name.
  assert(world.scene_window.tree_member.root.children[0].label=="Test");
}


static void testRemovingABody()
{
  Tester tester;
  FakeWorld &world = tester.world;
  FakeMainWindow &main_window = tester.main_window;
  FakeTreeEditor &tree_editor = main_window.tree_editor;
  FakeSceneWindow &scene_window = world.scene_window;
  size_t n_scene_properties = 1;

  // User executes Add Scene in the tree editor.
  tree_editor.userSelectsContextMenuItem("Add Scene");

  // User selects Add Body on the scene.
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");
  assert(world.sceneMember(0).scene.bodies().size()==1);
  assert(scene_window.tree_member.root.children.size()==1);

  scene_window.viewer_member.redraw_count = 0;

  // User selects Remove on the body.
  tree_editor.userSelectsContextMenuItem("Scene1|Body","Remove");

  // Assert that the body was removed from the scene.
  assert(world.sceneMember(0).scene.bodies().size()==0);

  // Assert the body was removed from the tree editor.
  assert(tree_editor.root.children[0].children.size()==n_scene_properties);

  // Assert the body was removed from the scene window tree.
  assert(scene_window.tree_member.root.children.size()==0);

  // Assert that the scene window viewer was redrawn.
  assert(scene_window.viewer_member.redraw_count==1);
}


static void testRemovingAPosExpr()
{
  Tester tester;
  FakeTreeEditor &tree_editor = tester.main_window.tree_editor;

  tree_editor.userSelectsContextMenuItem(
    "Add Charmapper"
  );
  tree_editor.userSelectsContextMenuItem(
    "Charmapper1","Add Motion Pass"
  );
  tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Motion Pass","Add Pos Expr"
  );
  tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Motion Pass|Pos Expr","Edit Diagram..."
  );
  tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Motion Pass|Pos Expr","Remove"
  );
}


static void testCancellingSaveProject()
{
  FakeMainWindow main_window;
  main_window.userPressesSaveProject(/*chosen_path*/"");
}


static void testCancellingOpenProject()
{
  FakeMainWindow main_window;
  main_window.userPressesOpenProject(/*chosen_path*/"");
}


#if 0
static void testCreatingABodyWithAnAveragePosition()
{
  Tester tester;
  //WorldWrapper &world_wrapper = tester.world_wrapper;
  FakeMainWindow &main_window = tester.main_window;
  FakeTreeEditor &tree_editor = main_window.tree_editor;

  tree_editor.userSelectsContextMenuItem("Add Scene");
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");
  tree_editor.userSelectsContextMenuItem("Scene1","Add Body");
  tree_editor.userSelectsContextMenuItem("Add Charmapper");
  tree_editor.userSelectsContextMenuItem("Charmapper1","Add Motion Pass");
  tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Motion Pass","Add Pos Expr"
  );
  tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Motion Pass|Pos Expr|Global Position","Edit Diagram..."
  );
  assert(tree_editor.diagram_editor_windows[0]);
  FakeDiagramEditor &diagram_editor =
    tree_editor.diagram_editor_windows[0]->diagram_editor;
  int body1_node_index =
    diagram_editor.userAddsANodeWithText("Scene1:Body1.position");
  int body2_node_index =
    diagram_editor.userAddsANodeWithText("Scene1:Body2.position");
  int add_node = diagram_editor.userAddsANodeWithText("$+$");
  diagram_editor.userConnects(body1_node_index,0,add_node,0);
  diagram_editor.userConnects(body2_node_index,0,add_node,1);
  int divide_node = diagram_editor.userAddsANodeWithText("$/2");
  diagram_editor.userConnects(add_node,0,divide_node,0);
  int return_node = diagram_editor.userAddsANodeWithText("return $");
  diagram_editor.userConnects(divide_node,0,return_node,0);

  tree_editor.userChangesNumberValue("Scene1|Body-1|position|x",2);
  tree_editor.userChangesNumberValue("Scene1|Body-2|position|x",4);

  FakeWorld &world = tester.world;;
  Scene::Body &body3 = world.sceneMember(0).scene.bodies()[2];
  Scene::Frame &frame = world.sceneMember(0).scene.displayFrame();
  assert(body3.position.x(frame)==3);
}
#endif


int main()
{
  testAddingABodyToTheScene();
  testAddingABodyToABody();
  testRemovingAMotionPass();
  testChangingABodyName();
  testRemovingABody();
  testRemovingAPosExpr();
  testCancellingSaveProject();
  testCancellingOpenProject();
  // testCreatingABodyWithAnAveragePosition();
}
