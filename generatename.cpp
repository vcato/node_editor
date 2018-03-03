#include "generatename.hpp"


using std::string;
using std::function;


string
  generateName(
    const string &prefix,
    function<bool(string)> name_exists_function
  )
{
  int number = 1;
  string name;

  for (;;) {
    name = prefix + std::to_string(number);

    if (!name_exists_function(name)) {
      break;
    }

    ++number;
  }

  return name;
}
