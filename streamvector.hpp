template <typename T>
std::ostream& operator<<(std::ostream &stream,const std::vector<T> &value)
{
  bool first = true;

  for (auto &x : value) {
    if (!first) {
      stream << ",";
    }
    else {
      first = false;
    }

    stream << x;
  }

  return stream;
}
