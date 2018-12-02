#include "executor.hpp"
#include "environment.hpp"


struct DiagramExecutor : Executor {
  Any return_value;
  std::ostream &show_stream;
  std::ostream &error_stream;

  DiagramExecutor(
    std::ostream &show_stream_arg,
    std::ostream &error_stream_arg,
    Environment *parent_environment_ptr = nullptr
  )
  : Executor(parent_environment_ptr),
    show_stream(show_stream_arg),
    error_stream(error_stream_arg)
  {
  }

  void executeShow(const Any& arg) override
  {
    show_stream << arg << "\n";
  }

  void executeReturn(const Any& arg) override
  {
    return_value = arg;
  }

  std::ostream& errorStream() override { return error_stream; }
};
