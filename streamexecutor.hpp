#include "executor.hpp"
#include "printonany.hpp"


struct StreamExecutor : Executor {
  std::ostream &stream;
  std::ostream &error_stream;

  StreamExecutor(std::ostream &stream_arg,std::ostream &error_stream_arg)
  : stream(stream_arg), error_stream(error_stream_arg)
  {
  }

  void print(float value)
  {
    stream << value;
  }

  void executeShow(const Any& value) override
  {
    print(value);
    stream << "\n";
  }

  void print(const std::vector<Any> &value)
  {
    printOn(stream,value);
  }

  void print(const Any &value)
  {
    printOn(stream,value);
  }

  void executeReturn(const Any& arg) override
  {
    std::cerr << "return ";
    printOn(std::cerr,arg);
    std::cerr << "\n";
  }

  virtual std::ostream& errorStream() { return error_stream; }
};
