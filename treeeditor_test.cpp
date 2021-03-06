#include "treeeditor.hpp"

#include <algorithm>
#include <sstream>
#include "wrapperutil.hpp"
#include "removefrom.hpp"
#include "fakediagrameditor.hpp"
#include "fakediagrameditorwindows.hpp"
#include "faketree.hpp"
#include "itemfrompath.hpp"
#include "observeddiagrams.hpp"
#include "ostreamvector.hpp"
#include "faketreeeditor.hpp"

using std::vector;
using std::istringstream;
using std::string;
using std::unique_ptr;
using std::make_unique;
using std::cerr;
using std::function;


namespace {
struct TestObject {
  enum class ValueType {
    void_,
    enumeration,
    numeric,
    string
  };

  TestObject *parent_ptr = nullptr;
  string label_member;
  vector<unique_ptr<TestObject>> children;
  Diagram *diagram_ptr = nullptr;
  ObservedDiagrams observed_diagrams;
  int *diagram_changed_count_ptr = nullptr;
  ValueType value_type = ValueType::void_;
  vector<string> enumeration_names;
  int enumeration_index = 0;
  NumericValue numeric_value = 0;
  string string_value = "";

  TestObject& createChild(const string &label)
  {
    children.push_back(make_unique<TestObject>());
    TestObject &child = *children.back();
    child.label_member = label;
    child.parent_ptr = this;
    return child;
  }

  void notifyDiagramChanged()
  {
    if (diagram_changed_count_ptr) {
      ++*diagram_changed_count_ptr;
    }
  }
};
}


namespace {
template <typename BaseType>
struct BasicTestWrapper : BaseType {
  using Label = typename BaseType::Label;
  using OperationName = typename BaseType::OperationName;

  BasicTestWrapper(TestObject &object_arg)
  : object(object_arg)
  {
  }

  BasicTestWrapper(const BasicTestWrapper &) = delete;

  Label label() const override
  {
    return object.label_member;
  }

  int nChildren() const override
  {
    return object.children.size();
  }

  void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const override;

  TestObject& createChild(const string &label)
  {
    return object.createChild(label);
  }

  virtual std::vector<OperationName> operationNames() const
  {
    return {"Remove","Replace"};
  }

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &handler
    ) const
  {
    if (operation_index==0) {
      assert(object.parent_ptr);
      handler.itemRemoved(path);
      removeFrom(object.parent_ptr->children,&object);
      return;
    }

    if (operation_index==1) {
      object.label_member = "c";
      handler.itemReplaced(path);
      return;
    }

    cerr << "operation_index: " << operation_index << "\n";
    assert(false);
  }

  Diagram *diagramPtr() const override { return object.diagram_ptr; }

  DiagramObserverPtr makeDiagramObserver() const override
  {
    assert(object.diagram_ptr);

    return
      object.observed_diagrams.makeObserver(
        *object.diagram_ptr,
        /*diagram_changed_hook*/ [&object = object](){
          object.notifyDiagramChanged();
        }
      );
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

typedef BasicTestWrapper<VoidWrapper> TestWrapper;


namespace {
struct VoidTestWrapper : BasicTestWrapper<VoidWrapper> {
  using BasicTestWrapper<VoidWrapper>::BasicTestWrapper;
};
}


namespace {
struct EnumerationTestWrapper : BasicTestWrapper<EnumerationWrapper> {
  using BasicTestWrapper<EnumerationWrapper>::BasicTestWrapper;

  std::vector<std::string> enumerationNames() const override
  {
    return object.enumeration_names;
  }

  void
    setValue(
      Index,
      const TreePath &,
      TreeObserver &
    ) const override
  {
    assert(false);
  }

  Index value() const override
  {
    return object.enumeration_index;
  }
};
}


namespace {
struct NumericTestWrapper : BasicTestWrapper<NumericWrapper> {
  using BasicTestWrapper<NumericWrapper>::BasicTestWrapper;

  void
    setValue(
      Value,
      const TreePath &,
      TreeObserver &
    ) const override
  {
    assert(false);
  }

  Value value() const override { return object.numeric_value; }
};
}


namespace {
struct StringTestWrapper : BasicTestWrapper<StringWrapper> {
  using BasicTestWrapper<StringWrapper>::BasicTestWrapper;

  Value value() const override
  {
    return object.string_value;
  }

  void setValue(const Value &,const TreePath &,TreeObserver &) const override
  {
    assert(false);
  }
};
}


template <typename BaseType>
void
  BasicTestWrapper<BaseType>::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  TestObject *child_ptr = object.children[child_index].get();

  assert(child_ptr);
  TestObject &child = *child_ptr;

  switch (child.value_type) {
    case TestObject::ValueType::void_:
      visitor(VoidTestWrapper(child));
      break;
    case TestObject::ValueType::enumeration:
      visitor(EnumerationTestWrapper(child));
      break;
    case TestObject::ValueType::numeric:
      visitor(NumericTestWrapper(child));
      break;
    case TestObject::ValueType::string:
      visitor(StringTestWrapper(child));
      break;
  }
}


static bool
  diagramIsObserved(
    ObservedDiagrams &observed_diagrams,
    const Diagram &diagram
  )
{
  ObservedDiagram *observed_diagram_ptr =
    observed_diagrams.findObservedDiagramFor(diagram);

  return observed_diagram_ptr != nullptr;
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
  assert(diagramIsObserved(object.observed_diagrams, diagram));

  editor.diagram_editor_windows[0]->userClosesWindow();

  assert(editor.diagram_editor_windows.empty());
  assert(editor.nDiagramEditorWindows()==0);

  assert(!diagramIsObserved(object.observed_diagrams, diagram));
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

  FakeTreeEditor tree_editor;
  tree_editor.setWorldPtr(&world);
  tree_editor.userSelectsContextMenuItem("member","Edit Diagram...");
  tree_editor.diagram_editor_windows[0]->callDiagramChangedCallback();
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
  const WrapperState &state = scan_result.asValue();
  editor.setWorldState(state);
  assert(!editor.tree_member.root.children.empty());
}


static void testReplacingAnItem(TestObject::ValueType value_type)
{
  // Create a test object
  // root
  //   a
  //   b
  TestObject object;
  TestObject &a = object.createChild("a");
  object.createChild("b");

  switch (value_type) {
    case TestObject::ValueType::void_:
      a.value_type = TestObject::ValueType::void_;
      break;
    case TestObject::ValueType::enumeration:
      a.value_type = TestObject::ValueType::enumeration;
      a.enumeration_names = {"x","y","z"};
      a.enumeration_index = 0;
      break;
    case TestObject::ValueType::numeric:
      a.value_type = TestObject::ValueType::numeric;
      a.numeric_value = 5.5;
      break;
    case TestObject::ValueType::string:
      a.value_type = TestObject::ValueType::string;
      a.string_value = "test";
      break;
  }

  // Create a tree editor
  FakeTreeEditor tree_editor;

  // Set the world wrapper on the tree editor.
  TestWrapper wrapper(object);
  tree_editor.setWorldPtr(&wrapper);

  FakeTree &tree = tree_editor.tree_member;
  assert(tree.root.children.size()==2);

  // Execute an operation which replaces item a
  tree_editor.userSelectsContextMenuItem("a","Replace");

  // Check that the item is replaced.
  assert(tree.root.children.size() == 2);
  assert(tree.root.children[0].label == "c");
}


static void testReplacingAVoidItem()
{
  testReplacingAnItem(TestObject::ValueType::void_);
}


static void testReplacingAnEnumerationItem()
{
  testReplacingAnItem(TestObject::ValueType::enumeration);
}


static void testReplacingANumericItem()
{
  testReplacingAnItem(TestObject::ValueType::numeric);
}


static void testReplacingAStringItem()
{
  testReplacingAnItem(TestObject::ValueType::string);
}


int main()
{
  testEditingDiagramThenClosingTheDiagramEditorWindow();
  testEditingDiagramThenRemovingItem();
  testEditingChildDiagramThenRemovingItem();
  testEditingDiagramNotifiesWrapper();
  testSettingWorldState();
  testReplacingAVoidItem();
  testReplacingAnEnumerationItem();
  testReplacingANumericItem();
  testReplacingAStringItem();
}
