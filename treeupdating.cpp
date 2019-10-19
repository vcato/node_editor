#include "treeupdating.hpp"

#include "streamvector.hpp"


using std::vector;
using std::string;
using std::cerr;


static vector<string> comboBoxItems(const EnumerationWrapper &wrapper)
{
  return wrapper.enumerationNames();
}


namespace {
struct CreateChildItemVisitor : Wrapper::SubclassVisitor {
  using LabelProperties = TreeWidget::LabelProperties;

  TreeWidget &tree;
  const TreePath &new_item_path;
  bool &created;

  static LabelProperties labelProperties(const Wrapper &wrapper)
  {
    return LabelProperties{/*text*/wrapper.label()};
  }

  CreateChildItemVisitor(
    TreeWidget &tree_arg,
    const TreePath &new_item_path_arg,
    bool &created_arg
  )
  : tree(tree_arg),
    new_item_path(new_item_path_arg),
    created(created_arg)
  {
  }

  void operator()(const VoidWrapper &wrapper) const override
  {
    tree.createVoidItem(
      new_item_path,
      labelProperties(wrapper)
    );
    created = true;
  }

  void operator()(const NumericWrapper &wrapper) const override
  {
    tree.createNumericItem(
      new_item_path,
      labelProperties(wrapper),
      wrapper.value(),
      wrapper.minimumValue(),
      wrapper.maximumValue()
    );
    created = true;
  }

  void operator()(const EnumerationWrapper &wrapper) const override
  {
    tree.createEnumerationItem(
      new_item_path,
      labelProperties(wrapper),
      comboBoxItems(wrapper),
      wrapper.value()
    );
    created = true;
  }

  void operator()(const StringWrapper &wrapper) const override
  {
    tree.createStringItem(
      new_item_path,
      labelProperties(wrapper),
      wrapper.value()
    );
    created = true;
  }
};
}


static void
  addWrapperItem(
    TreeWidget &tree,
    const TreePath &new_item_path,
    const Wrapper &wrapper
  )
{
  bool created = false;

  CreateChildItemVisitor
    create_child_item_visitor(tree,new_item_path,created);

  wrapper.accept(create_child_item_visitor);

  if (!created) {
    cerr << "No item created for " << new_item_path << "\n";
    assert(created);
  }
}


static void
  addMainTreeItem(
    TreeWidget &tree,
    const Wrapper &world,
    const TreePath &new_item_path
  )
{
  visitSubWrapper(
    world,
    new_item_path,
    [&](const Wrapper &w){
      addWrapperItem(tree,new_item_path,w);
    }
  );
}


static int nChildren(const Wrapper &wrapper,const TreePath &path)
{
  int n_children = 0;

  auto get_n_children_function =
    [&](const Wrapper &sub_wrapper){ n_children = sub_wrapper.nChildren(); };
  visitSubWrapper(wrapper,path,get_n_children_function);
  return n_children;
}


static void
  createTreeItem(
    TreeWidget &tree,const Wrapper &world,const TreePath &new_item_path
  );


void
  addChildTreeItems(
    TreeWidget &tree,
    const Wrapper &world,
    const TreePath &parent_path
  )
{
  int n_children = nChildren(world,parent_path);

  for (int i=0; i!=n_children; ++i) {
    createTreeItem(tree,world,childPath(parent_path,i));
  }
}


static void
  createTreeItem(
    TreeWidget &tree,const Wrapper &world,const TreePath &new_item_path
  )
{
  addMainTreeItem(tree,world,new_item_path);
  addChildTreeItems(tree,world,new_item_path);
}


static void
  updateItemLabel(
    TreeWidget &tree,
    const Wrapper &world,
    const TreePath &item_path
  )
{
  visitSubWrapper(
    world,
    item_path,
    [&](const Wrapper &w){
      tree.setItemLabel(item_path,w.label());
    }
  );
}


static void
  updateItemValue(
    TreeWidget &tree_widget,
    const Wrapper &world,
    const TreePath &item_path
  )
{
  struct Visitor : Wrapper::SubclassVisitor {
    TreeWidget &tree_widget;
    const TreePath &item_path;

    Visitor(
      TreeWidget &tree_widget_arg,
      const TreePath &item_path_arg
    )
    : tree_widget(tree_widget_arg),
      item_path(item_path_arg)
    {
    }

    void operator()(const VoidWrapper &) const override
    {
      assert(false);
    }

    void operator()(const NumericWrapper &wrapper) const override
    {
      tree_widget.setItemNumericValue(
        item_path,
        wrapper.value(),
        wrapper.minimumValue(),
        wrapper.maximumValue()
      );
    }

    void operator()(const EnumerationWrapper &) const override
    {
      assert(false);
    }

    void operator()(const StringWrapper &) const override
    {
      assert(false);
    }
  } visitor{tree_widget,item_path};

  visitSubWrapper(
    world,
    item_path,
    [&](const Wrapper &w){
      w.accept(visitor);
    }
  );
}


void TreeUpdatingObserver::itemAdded(const TreePath &path)
{
  createTreeItem(tree,world,path);
}


void TreeUpdatingObserver::itemReplaced(const TreePath &path)
{
  tree.removeItem(path);
  item_removed_function(path);
  createTreeItem(tree,world,path);
}


void TreeUpdatingObserver::itemRemoved(const TreePath &path)
{
  tree.removeItem(path);
  item_removed_function(path);
}


void TreeUpdatingObserver::itemLabelChanged(const TreePath &path)
{
  updateItemLabel(tree,world,path);
}


void TreeUpdatingObserver::itemValueChanged(const TreePath &path)
{
  updateItemValue(tree,world,path);
}
