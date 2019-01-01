#include "treeeditor.hpp"

#include <algorithm>
#include <sstream>
#include "wrapperutil.hpp"
#include "removefrom.hpp"
#include "fakediagrameditor.hpp"
#include "fakediagrameditorwindows.hpp"
#include "faketree.hpp"
#include "itemfrompath.hpp"

using std::vector;
using std::istringstream;
using std::string;
using std::unique_ptr;
using std::make_unique;


namespace {
struct FakeTreeEditor : TreeEditor {
  void removeTreeItem(const TreePath &/*path*/) override
  {
  }

  void removeChildItems(const TreePath &/*path*/) override
  {
  }

  void
    setItemExpanded(
      const TreePath &/*path*/,
      bool /*new_expanded_state*/
    ) override
  {
    // Our fake tree doesn't have expanded states.
  }

  int itemChildCount(const TreePath &parent_item) const override
  {
    return itemFromPath(tree.root,parent_item).children.size();
  }

  virtual void replaceTreeItems(const TreePath &/*parent_path*/)
  {
    assert(false);
  }

  void
    setEnumerationValues(
      const TreePath &,
      const std::vector<std::string> & /*items*/
    ) override
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
    if (parent_path.empty()) {
      tree.root.children.emplace_back();
      return;
    }

    assert(false);
  }

  void
    createNumericItem(
      const TreePath & /*parent_path*/,
      const std::string & /*label*/,
      const NumericValue /*value*/
    ) override
  {
    assert(false);
  }

  void
    createEnumerationItem(
      const TreePath &/*parent_path*/,
      const std::string &/*label*/,
      const std::vector<std::string> &/*options*/,
      int /*value*/
    ) override
  {
    assert(false);
  }

  void
    createStringItem(
      const TreePath &/*parent_path*/,
      const std::string &/*label*/,
      const std::string &/*value*/
    ) override
  {
    assert(false);
  }

  void
    userSelectsContextMenuItem(
      const string &path_string,
      const string &operation_name
    )
  {
    TreePath path = makePath(world(),path_string);

    if (operation_name=="Edit Diagram...") {
      openDiagramEditor(path);
    }
    else {
      executeOperation(path,operationIndex2(world(),path,operation_name));
    }
  }

  FakeDiagramEditorWindows diagram_editor_windows;
  FakeTree tree;
};
}


namespace {
struct TestObject {
  TestObject *parent_ptr = nullptr;
  string label_member;
  vector<unique_ptr<TestObject>> children;
  Diagram *diagram_ptr = nullptr;
  int *diagram_changed_count_ptr = nullptr;

  TestObject& createChild(const string &label)
  {
    children.push_back(make_unique<TestObject>());
    TestObject &child = *children.back();
    child.label_member = label;
    child.parent_ptr = this;
    return child;
  }
};
}


namespace {
struct TestWrapper : VoidWrapper {
  TestWrapper(TestObject &object_arg)
  : object(object_arg)
  {
  }

  TestWrapper(const TestWrapper &) = delete;

  virtual Label label() const
  {
    return object.label_member;
  }

  virtual int nChildren() const
  {
    return object.children.size();
  }

  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const
  {
    assert(object.children[child_index]);
    visitor(TestWrapper(*object.children[child_index]));
  }

  TestObject& createChild(const string &label)
  {
    return object.createChild(label);
  }

  virtual std::vector<OperationName> operationNames() const
  {
    return {"Remove"};
  }

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &handler
    ) const
  {
    assert(operation_index==0);
    assert(object.parent_ptr);
    handler.itemRemoved(path);
    removeFrom(object.parent_ptr->children,&object);
  }

  Diagram *diagramPtr() const override { return object.diagram_ptr; }

  void diagramChanged() const override
  {
    if (object.diagram_changed_count_ptr) {
      ++*object.diagram_changed_count_ptr;
    }
  }

  void setState(const WrapperState &new_state) const override
  {
    assert(new_state.value.isVoid());

    int n_new_children = new_state.children.size();

    for (int i=0; i!=n_new_children; ++i) {
      object.createChild(new_state.children[i].tag);
    }
  }

  TestObject &object;
};
}


static void testEditingDiagramThenClosingTheDiagramEditorWindow()
{
  Diagram diagram;
  TestObject object;
  TestWrapper world(object);
  object.diagram_ptr = &diagram;
  FakeTreeEditor editor;
  editor.setWorldPtr(&world);
  editor.userSelectsContextMenuItem("","Edit Diagram...");
  assert(editor.diagram_editor_windows[0]);
  assert(editor.nDiagramEditorWindows()==1);
  editor.diagram_editor_windows[0]->userClosesWindow();
  assert(editor.diagram_editor_windows.empty());
  assert(editor.nDiagramEditorWindows()==0);
}


static void testEditingDiagramThenRemovingItem()
{
  Diagram diagram;
  TestObject object;
  TestWrapper world(object);
  TestObject &child = world.createChild("child");
  child.diagram_ptr = &diagram;
  FakeTreeEditor editor;
  editor.setWorldPtr(&world);
  editor.userSelectsContextMenuItem("child","Edit Diagram...");
  editor.userSelectsContextMenuItem("child","Remove");
  assert(editor.nDiagramEditorWindows()==0);
}


static void testEditingChildDiagramThenRemovingItem()
{
  Diagram diagram;
  TestObject object;
  TestWrapper world(object);
  TestObject &parent = world.createChild("parent");
  TestObject &child = parent.createChild("child");
  child.diagram_ptr = &diagram;
  FakeTreeEditor editor;
  editor.setWorldPtr(&world);
  editor.userSelectsContextMenuItem("parent|child","Edit Diagram...");
  editor.userSelectsContextMenuItem("parent","Remove");
  assert(editor.nDiagramEditorWindows()==0);
}


static void testEditingDiagramNotifiesWrapper()
{
  Diagram diagram;
  TestObject object;
  TestWrapper world(object);
  TestObject &member = world.createChild("member");
  int diagram_changed_count = 0;
  member.diagram_ptr = &diagram;
  member.diagram_changed_count_ptr = &diagram_changed_count;
  FakeTreeEditor editor;
  editor.setWorldPtr(&world);
  editor.userSelectsContextMenuItem("member","Edit Diagram...");
  editor.diagram_editor_windows[0]->diagramChangedCallback()();
  assert(diagram_changed_count==1);
}


static void testSettingWorldState()
{
  TestObject object;
  TestWrapper world(object);
  FakeTreeEditor editor;
  editor.setWorldPtr(&world);

  const char *text =
    "world {\n"
    "  member: 5\n"
    "}\n";

  istringstream stream(text);
  ScanStateResult scan_result = scanStateFrom(stream);
  assert(!scan_result.isError());
  const WrapperState &state = scan_result.state();
  editor.setWorldState(state);
  assert(!editor.tree.root.children.empty());
}


int main()
{
  testEditingDiagramThenClosingTheDiagramEditorWindow();
  testEditingDiagramThenRemovingItem();
  testEditingChildDiagramThenRemovingItem();
  testEditingDiagramNotifiesWrapper();
  testSettingWorldState();
}
