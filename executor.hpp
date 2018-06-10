#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "any.hpp"

struct Executor {
  virtual void executeShow(const Any&) = 0;
  virtual void executeReturn(const Any&) = 0;
};


#endif /* EXECUTOR_HPP */
