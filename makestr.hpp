#include <string>
#include <sstream>


template <typename T>
inline std::string makeStr(const T &arg)
{
  std::ostringstream stream;
  stream << arg;
  return stream.str();
}
