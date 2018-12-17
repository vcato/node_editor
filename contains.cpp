#include "contains.hpp"

#include <algorithm>

template <typename T>
bool contains(const std::vector<T> &container,const T &value)
{
  return std::find(container.begin(),container.end(),value)!=container.end();
}


template bool contains<std::string>(const std::vector<std::string> &,const std::string &);
