#include "treeeditor.hpp"


using std::vector;
using std::string;
using std::cerr;


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


struct TreeEditor::OperationHandler : TreeOperationHandler {
  TreeEditor &tree_editor;

  OperationHandler(TreeEditor &tree_editor_arg)
  : tree_editor(tree_editor_arg)
  {
  }

  virtual void addItem(const TreePath &path)
  {
    tree_editor.addTreeItem(path);
  }

  virtual void replaceTreeItems(const TreePath &path)
  {
    tree_editor.replaceTreeItems(path);
  }

  virtual void changeEnumerationValues(const TreePath &path) const
  {
    tree_editor.changeEnumerationValues(path);
  }

  virtual void removeItem(const TreePath &path)
  {
    tree_editor.removeTreeItem(path);
  }
};


void TreeEditor::setEnumerationIndex(const TreePath &path,int index)
{
  OperationHandler operation_handler(*this);

  visitEnumerationSubWrapper(
    world(),
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      enumeration_wrapper.setValue(
        path,index,operation_handler
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
      OperationHandler operation_handler(*this);
      wrapper.executeOperation(operation_index,path,operation_handler);
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
