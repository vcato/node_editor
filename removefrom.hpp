#include <algorithm>

template <typename T>
void removeFrom(std::vector<T> &v,const T &arg)
{
  auto iter = std::find(begin(v),end(v),arg);
  assert(iter!=v.end());
  v.erase(iter);
}


template <typename T>
void removeFrom(std::vector<std::unique_ptr<T>> &v,const T *arg)
{
  auto is_same_pointer_function =
    [&](const std::unique_ptr<T> &ptr){
      return ptr.get()==arg;
    };

  auto iter = std::find_if(begin(v),end(v),is_same_pointer_function);

  assert(iter!=v.end());

  v.erase(iter);
}
