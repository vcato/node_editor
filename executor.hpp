#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "optional.hpp"
#include "any.hpp"

struct Executor {
  virtual void executeShow(const Any&) = 0;
  virtual void executeReturn(const Any&) = 0;
  virtual Optional<Any> variableValue(const std::string &name) const = 0;
};


#endif /* EXECUTOR_HPP */
