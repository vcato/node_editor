#include "executor.hpp"
#include "environment.hpp"
#include "diagramexecutioncontext.hpp"


struct DiagramExecutor : Executor {
  Optional<Any> maybe_return_value;
  std::ostream &show_stream;
  std::ostream &error_stream;
  Optional<std::string> optional_expected_return_type_name;

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
  bool tryExecuteReturn(const Any& arg, std::ostream &error_stream) override;

  std::ostream& errorStream() override { return error_stream; }
};
