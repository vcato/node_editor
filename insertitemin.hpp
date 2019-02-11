template <typename T>
T& insertItemIn(std::vector<T> &container,size_t index)
{
  container.emplace(container.begin() + index);
  return container[index];
}
