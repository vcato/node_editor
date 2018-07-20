#include "treeeditor.hpp"

#include <algorithm>
#include "wrapperutil.hpp"
#include "streamvector.hpp"
#include "removefrom.hpp"


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


struct TreeEditor::TreeObserver : ::TreeObserver {
  TreeEditor &tree_editor;

  TreeObserver(TreeEditor &tree_editor_arg)
  : tree_editor(tree_editor_arg)
  {
  }

  virtual void itemAdded(const TreePath &path)
  {
    tree_editor.addTreeItem(path);
  }

  virtual void itemReplaced(const TreePath &path)
  {
    tree_editor.replaceTreeItems(path);
  }

  virtual void enumarationValuesChanged(const TreePath &path) const
  {
    tree_editor.changeEnumerationValues(path);
  }

  virtual void itemRemoved(const TreePath &path)
  {
    tree_editor.removeTreeItem(path);
    tree_editor.removeDiagramEditors(path);
  }
};


void TreeEditor::setEnumerationIndex(const TreePath &path,int index)
{
  TreeObserver tree_observer(*this);

  visitEnumerationSubWrapper(
    world(),
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      enumeration_wrapper.setValue(
        path,index,tree_observer
      );
    }
  );
}


void TreeEditor::executeOperation(const TreePath &path,int operation_index)
{
  visitSubWrapper(
    world(),
    path,
    [&](const Wrapper &wrapper){
      TreeObserver tree_observer(*this);
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
      string_wrapper.setValue(value);
    }
  );
}


void TreeEditor::numberItemValueChanged(const TreePath &path,int value)
{
  visitNumericSubWrapper(
    world(),
    path,
    [&](const NumericWrapper &numeric_wrapper){
      numeric_wrapper.setValue(value);
    }
  );
}


#if 0
void TreeEditor::itemDiagramChanged(const TreePath &path)
{
  visitSubWrapper(
    world(),
    path,
    [&](const Wrapper &wrapper){
      wrapper.diagramChanged();
    }
  );
}
#endif


void TreeEditor::addMainTreeItem(const TreePath &new_item_path)
{
  visitSubWrapper(
    world(),
    new_item_path,
    [&](const Wrapper &w){
      addWrapperItem(new_item_path,w);
    }
  );
}


void TreeEditor::addTreeItem(const TreePath &new_item_path)
{
  addMainTreeItem(new_item_path);
  addChildTreeItems(new_item_path);
}


static int nChildren(const Wrapper &wrapper,const TreePath &path)
{
  int n_children = 0;

  auto get_n_children_function =
    [&](const Wrapper &sub_wrapper){ n_children = sub_wrapper.nChildren(); };
  visitSubWrapper(wrapper,path,get_n_children_function);
  return n_children;
}


void TreeEditor::addChildTreeItems(const TreePath &parent_path)
{
  int n_children = nChildren(world(),parent_path);

  for (int i=0; i!=n_children; ++i) {
    addTreeItem(join(parent_path,i));
  }
}


void TreeEditor::diagramEditorClosed(DiagramEditorWindow &window)
{
  removeFrom(diagram_editor_window_ptrs,&window);
}


void TreeEditor::notifyItemsOfDiagramChange(Diagram &diagram_that_changed)
{
  forEachDescendant(
    world(),
    [&](const Wrapper &wrapper){
      Diagram *diagram_ptr = wrapper.diagramPtr();

      if (diagram_ptr) {
        if (diagram_ptr==&diagram_that_changed) {
          wrapper.diagramChanged();
        }
      }
    }
  );
}


void TreeEditor::diagramChanged(DiagramEditorWindow &window)
{
  Diagram *diagram_ptr = window.diagramPtr();
  assert(diagram_ptr);
  notifyItemsOfDiagramChange(*diagram_ptr);
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

    window_ptr->close_callback = nullptr;
      // Make sure we don't get a callback

    window_ptr->forceClose();
    diagramEditorClosed(*window_ptr);
  }
}


void TreeEditor::openDiagramEditor(const TreePath &path)
{
  auto &window = createDiagramEditor();
  window.setDiagramPtr(diagramPtr(world(),path));
  diagram_editor_window_ptrs.push_back(&window);

  window.close_callback = [&]{ diagramEditorClosed(window); };
  window.diagramChangedCallback() = [&]{ diagramChanged(window); };
}


int TreeEditor::nDiagramEditorWindows() const
{
  return diagram_editor_window_ptrs.size();
}
