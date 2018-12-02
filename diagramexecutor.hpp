#include "executor.hpp"
#include "environment.hpp"


struct DiagramExecutionContext {
  std::ostream &show_stream;
  std::ostream &error_stream;
  Environment *parent_environment_ptr = nullptr;
};


struct DiagramExecutor : Executor {
  Any return_value;
  std::ostream &show_stream;
  std::ostream &error_stream;

  DiagramExecutor(const DiagramExecutionContext &context)
  : Executor(context.parent_environment_ptr),
    show_stream(context.show_stream),
    error_stream(context.error_stream)
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
