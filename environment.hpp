#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <map>
#include <string>
#include "any.hpp"


struct Environment {
  std::map<std::string,Any> map;
  Environment *parent_environment_ptr;

  Environment()
  : parent_environment_ptr(nullptr)
  {
  }

  Any& operator[](const std::string &name)
  {
    return map[name];
  }
};


inline Optional<Any>
  variableValue(const std::string &name,const Environment &environment)
{
  auto iter = environment.map.find(name);

  if (iter==environment.map.end()) {
    return {};
  }

  return iter->second;
}


#endif /* ENVIRONMENT_HPP */
