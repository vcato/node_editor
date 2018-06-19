#include "executor.hpp"
#include "printonany.hpp"


struct StreamExecutor : Executor {
  std::ostream &stream;

  StreamExecutor(std::ostream &stream_arg)
  : stream(stream_arg)
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

  Optional<Any> variableValue(const std::string &/*name*/) const override
  {
    return {};
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
};
