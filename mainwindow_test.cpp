#include "mainwindow.hpp"

#include <string>
#include <algorithm>
#include "world.hpp"
#include "worldwrapper.hpp"
#include "streamvector.hpp"
#include "wrapperutil.hpp"
#include "fakediagrameditorwindows.hpp"
#include "optional.hpp"
#include "scenewrapper.hpp"
#include "faketreeeditor.hpp"
#include "faketree.hpp"
#include "fakesceneviewer.hpp"

using std::string;
using std::vector;
using std::cerr;


namespace {
struct FakeMainWindow : MainWindow {
  FakeTreeEditor &treeEditor() override { return tree_editor; }

  Optional<std::string> _askForSavePath() override
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
struct FakeSceneTree : SceneTree, FakeTree {
  using Item = FakeTreeItem;

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
  using World::World;

  FakeSceneWindow scene_window;
  bool window_is_created = false;

  SceneWindow& createSceneViewerWindow(SceneMember &) override
  {
    assert(!window_is_created);
    window_is_created = true;
    return scene_window;
  }

  void destroySceneViewerWindow(SceneWindow &) override
  {
    assert(window_is_created);
    window_is_created = false;
  }
};
}


namespace {
struct Tester {
  FakeWorld world;
  WorldWrapper world_wrapper{world};
  FakeFiles files;
  FakeMainWindow main_window;

  Tester()
  {
    main_window.setWorldPtr(&world);
    main_window.setFileAccessorPtr(&files);
  }
};
}


static bool
  containsBody(const FakeSceneTree &scene_tree,const string &body_name)
{
  // This is over-specific.  It can be changed later if needed.
  return scene_tree.root.children[0].label == body_name;
}


static void openProjectWithText(Tester &tester,const char *test_project_text)
{
  tester.files.store("test.dat",test_project_text);
  tester.main_window.userPressesOpenProject("test.dat");
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
  assert(tree_editor.root().children[0].childCount()==0);
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
  size_t n_scene_properties = SceneWrapper::firstBodyChildIndex();

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
  assert(tree_editor.root().children[0].children.size()==n_scene_properties);

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


static void testEditingALocalPositionDiagram()
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
    "Charmapper1|Motion Pass|Pos Expr|Local Position","Edit Diagram..."
  );

  // Verify that the lineError() method returns something valid, even
  // though we haven't evaluated the local position diagram.
  assert(tree_editor.diagram_editor_windows[0]);
  FakeDiagramEditor &diagram_editor =
    tree_editor.diagram_editor_windows[0]->diagram_editor;
  assert(diagram_editor.lineError(/*node_index*/0,/*line_index*/0)=="");
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

  tree_editor.userChangesComboBoxIndex(
    "Charmapper1|Motion Pass|Pos Expr|Target Body",
    3 /* this should be Scene1:Body3 */
  );

  tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Motion Pass|Pos Expr|Global Position","Edit Diagram..."
  );
  assert(tree_editor.diagram_editor_windows[0]);
  FakeDiagramEditor &diagram_editor =
    tree_editor.diagram_editor_windows[0]->diagram_editor;
  int body1_node_index =
    diagram_editor.userAddsANodeWithText("Scene1.Body1.pos()");
  int body2_node_index =
    diagram_editor.userAddsANodeWithText("Scene1.Body2.pos()");
  int add_node = diagram_editor.userAddsANodeWithText("$+$");
  diagram_editor.userConnects(body1_node_index,0,add_node,0);
  diagram_editor.userConnects(body2_node_index,0,add_node,1);
  int divide_node = diagram_editor.userAddsANodeWithText("$/2");
  diagram_editor.userConnects(add_node,0,divide_node,0);
  int return_node = diagram_editor.userAddsANodeWithText("return $");
  diagram_editor.userConnects(divide_node,0,return_node,0);

  string background_frame_path = "Scene1|background_motion|0";
  tree_editor.userChangesNumberValue(background_frame_path + "|0",2);
  tree_editor.userChangesNumberValue(background_frame_path + "|2",4);

  FakeWorld &world = tester.world;
  Scene::Frame &frame = world.sceneMember(0).scene.displayFrame();
  Scene::Body &body1 = world.sceneMember(0).scene.bodies()[0];
  assert(body1.position_map.x(frame)==2);
  Scene::Body &body2 = world.sceneMember(0).scene.bodies()[1];
  assert(body2.position_map.x(frame)==4);

  Scene::Body &body3 = world.sceneMember(0).scene.bodies()[2];

  // Body 3's x value should be 3, since we should be calculating it as
  // (2+4)/2.
  assert(body3.position_map.x(frame)==3);
}


static void testOpeningAProject()
{
  const char *test_project_text =
    "world {\n"
    "  scene1 {\n"
    "    background_motion {\n"
    "      0 {\n"
    "        0: 48\n"
    "        1: 0\n"
    "      }\n"
    "    }\n"
    "    body {\n"
    "      name: \"Body1\"\n"
    "      position_map {\n"
    "        x_variable: 0\n"
    "        y_variable: 1\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n";

  Tester tester;
  openProjectWithText(tester,test_project_text);

  FakeTreeEditor &tree_editor = tester.main_window.tree_editor;
  assert(!tree_editor.root().children[0].is_expanded);
  assert(!tree_editor.root().children[0].children[0].is_expanded);
}


static void testOpeningAProjectWhenADiagramEditorIsOpen()
{
  const char *test_project_text =
    "world {\n"
    "  charmapper {\n"
    "    variable_pass {\n"
    "      var: 0 {\n"
    "        name: \"x\"\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n";

  // Have some world where we have something that has a diagram.
  Tester tester;
  openProjectWithText(tester,test_project_text);

  // User opens the diagram editor.
  tester.main_window.tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Variable Pass|x",
    "Edit Diagram..."
  );

  assert( tester.main_window.tree_editor.diagram_editor_windows.size() == 1);

  // User re-opens the same project.
  openProjectWithText(tester,test_project_text);

  assert( tester.main_window.tree_editor.diagram_editor_windows.size() == 0);
}


static void testChangingAVariableName()
{
  // Have a charmapper with a variable pass with a variable.
  // Change the name of the variable.
  // Make sure the label in the tree shows the new name.

  const char *test_project_text =
    "world {\n"
    "  charmapper {\n"
    "    variable_pass {\n"
    "      var: 0 {\n"
    "        name: \"x\"\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n";

  Tester tester;
  openProjectWithText(tester,test_project_text);

  tester.main_window.tree_editor.userChangesStringValue(
    "Charmapper1|Variable Pass|x|name",
    "y"
  );

  FakeTreeItem &var_item =
    tester.main_window.tree_editor.root().children[0].children[0].children[0];
  assert(var_item.label == "y");
}


static void testChangingAVariableRange()
{
  const char *test_project_text =
    "world {\n"
    "  charmapper {\n"
    "    variable_pass {\n"
    "      var: 0 {\n"
    "        name: \"x\"\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n";

  Tester tester;
  openProjectWithText(tester,test_project_text);

  tester.main_window.tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Variable Pass|x",
    "Add Minimum"
  );

  tester.main_window.tree_editor.userChangesNumberValue(
    "Charmapper1|Variable Pass|x|minimum",
    10
  );

  tester.main_window.tree_editor.userSelectsContextMenuItem(
    "Charmapper1|Variable Pass|x",
    "Add Maximum"
  );

  tester.main_window.tree_editor.userChangesNumberValue(
    "Charmapper1|Variable Pass|x|maximum",
    20
  );

  FakeTreeItem &variable_item =
    tester.main_window.tree_editor.root().children[0].children[0].children[0];
  FakeSlider *slider_ptr = variable_item.maybeSlider();
  assert(slider_ptr);
  assert(slider_ptr->minimum == 10);
  assert(slider_ptr->maximum == 20);
}


static void testMovingABodyInTheSceneViewer()
{
  FakeWorld world;
  Scene &scene = world.addScene();
  Scene::Body &body = scene.addBody();
  FakeMainWindow main_window;
  main_window.setWorldPtr(&world);
  FakeSceneViewer &viewer = world.scene_window.viewer_member;

  ViewportPoint body_center = viewer.centerOfBody(body);

  viewer.userPressesMouseAt(body_center);
  viewer.userMovesMouseTo(body_center + ViewportVector(10,0));
  viewer.userReleasesMouse();

  float x = body.position_map.x(scene.backgroundFrame());
  float y = body.position_map.y(scene.backgroundFrame());

  assert(Point2D(x,y) == Point2D(10,0));
}


int main()
{
  testAddingABodyToTheScene();
  testAddingABodyToABody();
  testRemovingAMotionPass();
  testChangingABodyName();
  testRemovingABody();
  testRemovingAPosExpr();
  testEditingALocalPositionDiagram();
  testCancellingSaveProject();
  testCancellingOpenProject();
  testCreatingABodyWithAnAveragePosition();
  testOpeningAProject();
  testOpeningAProjectWhenADiagramEditorIsOpen();
  testChangingAVariableName();
  testChangingAVariableRange();
  testMovingABodyInTheSceneViewer();
}
