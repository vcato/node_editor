#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "optional.hpp"
#include "any.hpp"
#include "environment.hpp"

struct Executor {
  const Environment *parent_environment_ptr;
  std::ostream &debug_stream;

  virtual void executeShow(const Any&) = 0;
  virtual bool tryExecuteReturn(const Any&,std::ostream &error_stream) = 0;

  std::ostream& debugStream() { return debug_stream; }

  Executor(
    const Environment *parent_environment_ptr_arg,
    std::ostream &debug_stream_arg
  )
  : parent_environment_ptr(parent_environment_ptr_arg),
    debug_stream(debug_stream_arg)
  {
  }

  Optional<Any> variableValue(const std::string &name) const
  {
    return ::variableValue(name,parent_environment_ptr);
  }
};


#endif /* EXECUTOR_HPP */
