#include "treeeditor.hpp"

#include <algorithm>
#include "wrapperutil.hpp"
#include "removefrom.hpp"
#include "fakediagrameditor.hpp"
#include "fakediagrameditorwindows.hpp"

using std::vector;
using std::string;
using std::unique_ptr;
using std::make_unique;


namespace {
struct FakeTreeEditor : TreeEditor {
  void removeTreeItem(const TreePath &/*path*/) override
  {
  }

  virtual void addMainTreeItem(const TreePath &/*new_item_path*/)
  {
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
};
}


namespace {
struct TestWrapper : VoidWrapper {
  virtual Label label() const
  {
    return label_member;
  }

  virtual int nChildren() const
  {
    return children.size();
  }

  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const
  {
    assert(children[child_index]);
    visitor(*children[child_index]);
  }

  TestWrapper& createChild(const string &label)
  {
    children.push_back(make_unique<TestWrapper>());
    TestWrapper &child = *children.back();
    child.label_member = label;
    child.parent_ptr = this;
    return child;
  }

  virtual std::vector<OperationName> operationNames() const
  {
    return {"Remove"};
  }

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      OperationHandler &handler
    ) const
  {
    assert(operation_index==0);
    assert(parent_ptr);
    handler.removeItem(path);
    removeFrom(parent_ptr->children,this);
  }

  Diagram *diagramPtr() const override { return diagram_ptr; }

  TestWrapper *parent_ptr = nullptr;
  vector<unique_ptr<TestWrapper>> children;
  Diagram *diagram_ptr = nullptr;
  string label_member;
};
}


static void testEditingDiagramThenClosingTheDiagramEditorWindow()
{
  Diagram diagram;
  TestWrapper world;
  world.diagram_ptr = &diagram;
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
  TestWrapper world;
  TestWrapper &child = world.createChild("child");
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
  TestWrapper world;
  TestWrapper &parent = world.createChild("parent");
  TestWrapper &child = parent.createChild("child");
  child.diagram_ptr = &diagram;
  FakeTreeEditor editor;
  editor.setWorldPtr(&world);
  editor.userSelectsContextMenuItem("parent|child","Edit Diagram...");
  editor.userSelectsContextMenuItem("parent","Remove");
  assert(editor.nDiagramEditorWindows()==0);
}


int main()
{
  testEditingDiagramThenClosingTheDiagramEditorWindow();
  testEditingDiagramThenRemovingItem();
  testEditingChildDiagramThenRemovingItem();
}
