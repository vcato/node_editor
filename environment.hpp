#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <map>
#include <string>
#include "any.hpp"

using Environment = std::map<std::string,Any>;


inline Optional<Any>
  variableValue(const std::string &name,const Environment &environment)
{
  auto iter = environment.find(name);

  if (iter==environment.end()) {
    return {};
  }

  return iter->second;
}

#endif /* ENVIRONMENT_HPP */
