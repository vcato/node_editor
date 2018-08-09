#include "executor.hpp"
#include "environment.hpp"


struct DiagramExecutor : Executor {
  Any return_value;
  std::ostream &show_stream;

  DiagramExecutor(std::ostream &show_stream_arg)
  : show_stream(show_stream_arg)
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
};
