#ifndef FAKETREE_HPP_
#define FAKETREE_HPP_


#include <string>
#include <vector>


struct FakeTreeItem {
  bool is_expanded;
  std::string label;
  std::vector<FakeTreeItem> children;

  FakeTreeItem()
  : is_expanded(true)
  {
  }

  int childCount() const { return children.size(); }
  FakeTreeItem *child(int index) { return &children[index]; }
  const FakeTreeItem *child(int index) const { return &children[index]; }

  friend FakeTreeItem& insertChildItem(FakeTreeItem &parent_item,int index)
  {
    parent_item.children.insert(
      parent_item.children.begin()+index,
      FakeTreeItem()
    );

    return parent_item.children[index];
  }

  friend void removeChildItem(FakeTreeItem &parent_item,int index)
  {
    parent_item.children.erase(parent_item.children.begin() + index);
  }

  friend void setText(FakeTreeItem &item,const std::string &text)
  {
    item.label = text;
  }
};


struct FakeTree {
  using Item = FakeTreeItem;

  Item root;
};


#endif /* FAKETREE_HPP_ */
