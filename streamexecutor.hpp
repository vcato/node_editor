#include "executor.hpp"


struct StreamExecutor : Executor {
  std::ostream &stream;
  std::ostream &error_stream;

  StreamExecutor(std::ostream &stream_arg,std::ostream &error_stream_arg)
  : Executor(/*parent_environment_ptr*/0),
    stream(stream_arg),
    error_stream(error_stream_arg)
  {
  }

  void executeShow(const Any& value) override
  {
    print(value);
    stream << "\n";
  }

  bool tryExecuteReturn(const Any& arg,std::ostream &error_stream) override;

  void print(float value)
  {
    stream << value;
  }

  void print(const std::vector<Any> &value);

  void print(const Any &value);

  virtual std::ostream& errorStream() { return error_stream; }
};
