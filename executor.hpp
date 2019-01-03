#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "optional.hpp"
#include "any.hpp"
#include "environment.hpp"


struct Executor {
  const Environment *parent_environment_ptr;

  virtual void executeShow(const Any&) = 0;
  virtual void executeReturn(const Any&) = 0;
  virtual std::ostream& errorStream() = 0;

  Executor(const Environment *parent_environment_ptr_arg)
  : parent_environment_ptr(parent_environment_ptr_arg)
  {
  }

  Optional<Any> variableValue(const std::string &name) const
  {
    return ::variableValue(name,parent_environment_ptr);
  }
};


#endif /* EXECUTOR_HPP */
