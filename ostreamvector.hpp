template <typename T>
static std::ostream&
  operator<<(std::ostream &stream,const std::vector<T> &value)
{
  stream << "[";

  if (!value.empty()) {
    auto iter = value.begin();

    stream << *iter++;

    while (iter!=value.end()) {
      stream << ",";
      stream << *iter++;
    }
  }

  stream << "]";

  return stream;
}
