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

using std::vector;
using std::istringstream;
using std::string;
using std::unique_ptr;
using std::make_unique;
using std::cerr;


template <typename T>
static void removeIndexFrom(vector<T> &container,size_t index)
{
  container.erase(container.begin() + index);
}


template <typename T>
static T& insertItemIn(vector<T> &container,size_t index)
{
  container.emplace(container.begin() + index);
  FakeTree::Item &new_item = container[index];
  return new_item;
}


static void
  createItem(
    FakeTree &tree,
    const TreePath &new_item_path,
    const string &label
  )
{
  TreePath parent_path = parentPath(new_item_path);
  FakeTree::Item &parent_item = itemFromPath(tree.root, parent_path);
  int new_item_index = new_item_path.back();
  FakeTree::Item &new_item =
    insertItemIn(parent_item.children,new_item_index);
  new_item.label = label;
}


namespace {
struct FakeTreeEditor : TreeEditor {
  void removeTreeItem(const TreePath &path) override
  {
    FakeTree::Item &parent_item = itemFromPath(tree.root, parentPath(path));
    removeIndexFrom(parent_item.children, path.back());
  }

  void removeChildItems(const TreePath &path) override
  {
    if (path.empty()) {
      tree.root.children.clear();
      return;
    }

    cerr << "path: " << path << "\n";
    assert(false);
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
    return itemFromPath(tree.root, parent_item).children.size();
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
      const TreePath &,
      const TreePath &new_item_path,
      const std::string &label
    ) override
  {
    createItem(tree,new_item_path,label);
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
      const TreePath &parent_path,
      const TreePath &new_item_path,
      const std::string &label,
      const std::vector<std::string> &/*options*/,
      int /*value*/
    ) override
  {
    assert(parentPath(new_item_path) == parent_path);
    createItem(tree,new_item_path,label);
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


#if 0
namespace {
struct VoidTestValue {
  static TestValueType type() const { return void_; }
};
}


namespace {
struct EnumerationTestValue {
  static TestValueType type() const { return enumeration; }
}
}


namespace {
struct TestValue {
  struct Data {
    virtual Type type() const = 0;
  };

  template <typename T>
  struct BasicData : Data {
    T value;

    Type type() const override { return value.type(); }
  };

  unique_ptr<Data> data_ptr;

  template <typename T>
  TestValue(const T &arg)
  : data_ptr(make_unique<BasicData<T>>(arg))
  {
  }
};
}
#endif


namespace {
struct TestObject {
  enum class ValueType {
    void_,
    enumeration
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
    return object.observed_diagrams.makeObserver(*object.diagram_ptr);
  }

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

typedef BasicTestWrapper<VoidWrapper> TestWrapper;

struct VoidTestWrapper : BasicTestWrapper<VoidWrapper> {
  using BasicTestWrapper<VoidWrapper>::BasicTestWrapper;
};


struct EnumerationTestWrapper : BasicTestWrapper<EnumerationWrapper> {
  using BasicTestWrapper<EnumerationWrapper>::BasicTestWrapper;

  std::vector<std::string> enumerationNames() const override
  {
    return object.enumeration_names;
  }

  void
    setValue(
      const TreePath &,
      Index,
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



template <typename BaseType>
void
  BasicTestWrapper<BaseType>::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  assert(object.children[child_index]);
  if (
    object.children[child_index]->value_type==TestObject::ValueType::void_) {

    visitor(VoidTestWrapper(*object.children[child_index]));
  }
  else {
    // This needs to be a different type of TestWrapper
    visitor(EnumerationTestWrapper(*object.children[child_index]));
  }
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


static void testReplacingAVoidItem()
{
  // Create a test object
  // root
  //   a
  //   b
  TestObject object;
  object.createChild("a");
  object.createChild("b");

  // Create a tree editor
  FakeTreeEditor tree_editor;

  // Set the world wrapper on the tree editor.
  TestWrapper wrapper(object);
  tree_editor.setWorldPtr(&wrapper);

  assert(tree_editor.tree.root.children.size()==2);

  // Execute an operation which replaces item a
  tree_editor.userSelectsContextMenuItem("a","Replace");

  // Check that the item is replaced.
  assert(tree_editor.tree.root.children.size() == 2);
  assert(tree_editor.tree.root.children[0].label == "c");
}


static void testReplacingAnEnumerationItem()
{
  // Create a test object
  // root
  //   a
  //   b
  TestObject object;
  TestObject &a = object.createChild("a");
  object.createChild("b");

  a.value_type = TestObject::ValueType::enumeration;
  a.enumeration_names = {"x","y","z"};
  a.enumeration_index = 0;

  // Create a tree editor
  FakeTreeEditor tree_editor;

  // Set the world wrapper on the tree editor.
  TestWrapper wrapper(object);
  tree_editor.setWorldPtr(&wrapper);

  assert(tree_editor.tree.root.children.size()==2);

  // Execute an operation which replaces item a
  tree_editor.userSelectsContextMenuItem("a","Replace");

  // Check that the item is replaced.
  assert(tree_editor.tree.root.children.size() == 2);
  assert(tree_editor.tree.root.children[0].label == "c");
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
}
