inline std::ostream&
  operator<<(std::ostream &stream,const std::vector<int> &value)
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
