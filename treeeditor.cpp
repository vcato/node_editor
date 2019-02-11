#include "treeeditor.hpp"

#include <algorithm>
#include "wrapperutil.hpp"
#include "streamvector.hpp"
#include "removefrom.hpp"
#include "treeupdating.hpp"

using std::vector;
using std::string;
using std::cerr;
using std::function;


static void
  forEachDescendant(const Wrapper &wrapper,function<void(const Wrapper &)> f)
{
  int n_children = wrapper.nChildren();

  for (int i=0; i!=n_children; ++i) {
    wrapper.withChildWrapper(i,[&](const Wrapper &child_wrapper){
      forEachDescendant(child_wrapper,f);
    });
  }

  f(wrapper);
}


void TreeEditor::setWorldPtr(Wrapper *arg)
{
  removeChildItems(TreePath());
  world_ptr = arg;
  addChildTreeItems(tree(),world(),TreePath());
}


Wrapper &TreeEditor::world()
{
  assert(world_ptr);
  return *world_ptr;
}


vector<string> TreeEditor::operationNames(const TreePath &path)
{
  vector<string> operation_names;

  visitSubWrapper(world(),path,[&](const Wrapper &sub_wrapper){
    operation_names = sub_wrapper.operationNames();
  });

  return operation_names;
}


void TreeEditor::setEnumerationIndex(const TreePath &path,int index)
{
  visitEnumerationSubWrapper(
    world(),
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      TreeUpdatingObserver tree_observer = treeObserver(*this);

      enumeration_wrapper.setValue(
        index,path,tree_observer
      );
    }
  );
}


namespace {
struct TreeObserverStub : TreeObserver {
  virtual void itemAdded(const TreePath &)
  {
  }

  virtual void itemReplaced(const TreePath &)
  {
  }

  virtual void itemRemoved(const TreePath &)
  {
  }

  virtual void enumerationValuesChanged(const TreePath &) const
  {
  }
};
}


void TreeEditor::replaceChildTreeItems(const TreePath &parent_path)
{
  removeChildItems(parent_path);
  addChildTreeItems(tree(),world(),parent_path);
}


void TreeEditor::collapseBranch(const TreePath &path)
{
  setItemExpanded(path,false);

  collapseChildren(path);
}


void TreeEditor::collapseChildren(const TreePath &path)
{
  int n_children = itemChildCount(path);

  for (int i=0; i!=n_children; ++i) {
    collapseBranch(childPath(path,i));
  }
}


TreeUpdatingObserver TreeEditor::treeObserver(TreeEditor &tree_editor)
{
  return
    TreeUpdatingObserver(
      tree_editor.tree(),
      tree_editor.world(),
      [&tree_editor](const TreePath &tree_path){
        tree_editor.removeDiagramEditors(tree_path);
      }
    );
}


void TreeEditor::setWorldState(const WrapperState &new_state)
{
  Wrapper *world_ptr = worldPtr();
  assert(world_ptr);
  removeDiagramEditors(TreePath());
  world_ptr->setState(new_state);
  replaceChildTreeItems(TreePath());
  collapseChildren(TreePath());
}


void TreeEditor::executeOperation(const TreePath &path,int operation_index)
{
  visitSubWrapper(
    world(),
    path,
    [&](const Wrapper &wrapper){
      TreeUpdatingObserver tree_observer = treeObserver(*this);
      wrapper.executeOperation(operation_index,path,tree_observer);
    }
  );
}


void
  TreeEditor::stringItemValueChanged(
    const TreePath &path,const string &value
  )
{
  visitStringSubWrapper(
    world(),
    path,
    [&](const StringWrapper &string_wrapper){
      TreeUpdatingObserver tree_observer = treeObserver(*this);
      string_wrapper.setValue(value,path,tree_observer);
    }
  );
}


void TreeEditor::numberItemValueChanged(const TreePath &path,int value)
{
  visitNumericSubWrapper(
    world(),
    path,
    [&](const NumericWrapper &numeric_wrapper){
      TreeUpdatingObserver tree_observer = treeObserver(*this);
      numeric_wrapper.setValue(value,path,tree_observer);
    }
  );
}


const Optional<TreePath> &TreeEditor::maybePathOfItemBeingEdited() const
{
  return maybe_path_of_item_being_edited;
}


void TreeEditor::diagramEditorClosed(DiagramEditorWindow &window)
{
  removeFrom(diagram_editor_window_ptrs,&window);
}


void TreeEditor::removeDiagramEditors(const TreePath &path)
{
  if (diagram_editor_window_ptrs.empty()) {
    return;
  }

  // Find the diagrams

  vector<Diagram *> diagrams_being_removed;

  visitSubWrapper(world(),path,[&](const Wrapper &wrapper){
    forEachDescendant(wrapper,
      [&](const Wrapper &wrapper){
        Diagram *diagram_ptr = wrapper.diagramPtr();

        if (diagram_ptr) {
          diagrams_being_removed.push_back(diagram_ptr);
        }
      }
    );
  });

  vector<DiagramEditorWindow *> windows_to_close;

  for (Diagram *diagram_ptr : diagrams_being_removed) {
    for (auto window_ptr : diagram_editor_window_ptrs) {
      assert(window_ptr);

      if (window_ptr->diagramPtr()==diagram_ptr) {
        windows_to_close.push_back(window_ptr);
      }
    }
  }

  for (auto window_ptr : windows_to_close) {
    assert(window_ptr);

    window_ptr->closeCallback() = nullptr;
      // Make sure we don't get a callback

    window_ptr->forceClose();
    diagramEditorClosed(*window_ptr);
  }
}


void TreeEditor::openDiagramEditor(const TreePath &path)
{
  auto &window = createDiagramEditor();
  window.setDiagramObserver(diagramObserverPtr(world(),path));
  diagram_editor_window_ptrs.push_back(&window);

  window.closeCallback() = [&]{ diagramEditorClosed(window); };
}


int TreeEditor::nDiagramEditorWindows() const
{
  return diagram_editor_window_ptrs.size();
}


auto TreeEditor::contextMenuItems(const TreePath &path) -> vector<MenuItem>
{
  vector<MenuItem> menu_items;

  if (canEditDiagramOf(world(),path)) {
    menu_items.push_back(
      {"Edit Diagram...",[this,path]{ openDiagramEditor(path); }}
    );
  }

  std::vector<std::string> operation_names = operationNames(path);

  int n_operations = operation_names.size();

  for (int i=0; i!=n_operations; ++i) {
    menu_items.push_back(
      {operation_names[i],[this,path,i]{ executeOperation(path,i); }}
    );
  }

  return menu_items;
}
