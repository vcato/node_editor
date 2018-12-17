#include <vector>
#include <string>


template <typename T>
bool contains(const std::vector<T> &container,const T &value);

extern template
bool contains<std::string>(const std::vector<std::string> &,const std::string &);
