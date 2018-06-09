#include "executor.hpp"


struct StreamExecutor : Executor {
  std::ostream &stream;

  StreamExecutor(std::ostream &stream_arg)
  : stream(stream_arg)
  {
  }

  void executeShow(float value)
  {
    stream << value << "\n";
  }

  virtual void executeReturn(float)
  {
    assert(false);
  }
};
