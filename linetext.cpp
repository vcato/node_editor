#include "linetext.hpp"


bool lineTextHasInput(const std::string &text)
{
  if (endsWith(text,"$")) {
    return true;
  }

  if (contains(text,"$)")) {
    return true;
  }

  return false;
}


bool lineTextHasOutput(const std::string &text)
{
  if (startsWith(text,"$")) {
    return true;
  }

  return false;
}
