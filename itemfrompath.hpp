template <typename Item>
inline const Item &
  itemFromPath(const Item &root_item,const std::vector<int> &path)
{
  const Item *item_ptr = &root_item;
  int path_size = path.size();

  for (int depth = 0; depth<path_size; ++depth) {
    item_ptr = item_ptr->child(path[depth]);
  }

  assert(item_ptr);
  return *item_ptr;
}


template <typename Item>
inline Item &
  itemFromPath(Item &root_item,const std::vector<int> &path)
{
  const Item &const_root_item = root_item;
  const Item &const_result = itemFromPath(const_root_item,path);
  return const_cast<Item&>(const_result);
}
