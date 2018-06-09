#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP


struct Executor {
  virtual void executeShow(float) = 0;
  virtual void executeReturn(float) = 0;
};


#endif /* EXECUTOR_HPP */
