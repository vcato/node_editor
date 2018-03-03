#include <string>
#include <functional>


extern std::string
  generateName(
    const std::string &prefix,
    std::function<bool(std::string)> name_exists_function
  );
