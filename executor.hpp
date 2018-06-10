#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "any.hpp"

struct Executor {
  virtual void executeShow(float) = 0;
  virtual void executeShow(const std::vector<Any> &) = 0;
  virtual void executeReturn(float) = 0;
  virtual void executeReturn(const std::vector<Any> &) = 0;
};


#endif /* EXECUTOR_HPP */
