#include "treeeditor.hpp"

#include <algorithm>
#include <sstream>
#include "wrapperutil.hpp"
#include "removefrom.hpp"
#include "fakediagrameditor.hpp"
#include "fakediagrameditorwindows.hpp"
#include "faketree.hpp"

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

  virtual void removeChildItems(const TreePath &/*path*/)
  {
  }

  void
    addWrapperItem(const TreePath &new_item_path,const Wrapper &) override
  {
    if (new_item_path.empty()) {
      assert(false);
    }

    int n_children = tree.children.size();

    if (new_item_path.size()==1 && n_children==new_item_path.back()) {
      tree.children.emplace_back();
      return;
    }

    assert(false);
  }

  virtual void replaceTreeItems(const TreePath &/*parent_path*/)
  {
    assert(false);
  }

  virtual void changeEnumerationValues(const TreePath &)
  {
    assert(false);
  }

  virtual DiagramEditorWindow& createDiagramEditor()
  {
    return diagram_editor_windows.create();
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

  void
    setState(
      const WrapperState &new_state,
      const TreePath &tree_path,
      TreeObserver &tree_observer
    ) const override
  {
    assert(new_state.value.isVoid());

    int n_new_children = new_state.children.size();

    for (int i=0; i!=n_new_children; ++i) {
      object.createChild(new_state.children[i].tag);
      tree_observer.itemAdded(join(tree_path,i));
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
  assert(!editor.tree.children.empty());
}


int main()
{
  testEditingDiagramThenClosingTheDiagramEditorWindow();
  testEditingDiagramThenRemovingItem();
  testEditingChildDiagramThenRemovingItem();
  testEditingDiagramNotifiesWrapper();
  testSettingWorldState();
}
