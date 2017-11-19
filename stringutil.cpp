#include "stringutil.hpp"


bool endsWith(const std::string &text,const std::string &suffix)
{
  if (suffix.size()>text.size()) return false;
  if (std::string(text.end()-suffix.size(),text.end())==suffix) return true;
  return false;
}


bool startsWith(const std::string &text,const std::string &prefix)
{
  if (prefix.size()>text.size()) return false;
  if (std::string(text.begin(),text.begin()+prefix.size())==prefix) return true;
  return false;
}


bool contains(const std::string &text,const std::string &contents)
{
  size_t position = text.find(contents);
  if (position==text.npos) {
    return false;
  }
  return true;
}
