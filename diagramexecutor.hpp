#include "executor.hpp"
#include "environment.hpp"
#include "diagramexecutioncontext.hpp"


struct DiagramExecutor : Executor {
  Optional<Any> maybe_return_value;
  std::ostream &show_stream;
  std::ostream &error_stream;

  DiagramExecutor(
    const DiagramExecutionContext &context,
    const Environment *parent_environment_ptr
  )
  : Executor(parent_environment_ptr),
    show_stream(context.show_stream),
    error_stream(context.error_stream)
  {
  }

  void executeShow(const Any& arg) override;

  void executeReturn(const Any& arg) override
  {
    maybe_return_value = arg;
  }

  std::ostream& errorStream() override { return error_stream; }
};
