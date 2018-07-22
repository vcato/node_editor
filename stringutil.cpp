#include "stringutil.hpp"

#include <cassert>


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


bool startsWith(const std::string &text,char c)
{
  if (text.length()==0) {
    assert(false);
  }

  return text[0]==c;
}


bool contains(const std::string &text,const std::string &contents)
{
  size_t position = text.find(contents);
  if (position==text.npos) {
    return false;
  }
  return true;
}


std::string withoutRight(const std::string &text,size_t n)
{
  assert(text.length()>=n);
  return text.substr(0,text.length()-n);
}
