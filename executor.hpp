#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "optional.hpp"
#include "any.hpp"
#include "environment.hpp"


struct Executor {
  Environment environment;

  virtual void executeShow(const Any&) = 0;
  virtual void executeReturn(const Any&) = 0;
  virtual std::ostream& errorStream() = 0;

  Optional<Any> variableValue(const std::string &name) const
  {
    return ::variableValue(name,environment);
  }
};


#endif /* EXECUTOR_HPP */
