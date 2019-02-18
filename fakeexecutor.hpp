#include <sstream>
#include "executor.hpp"


struct FakeExecutor : Executor {
  std::ostringstream &execution_stream;
  std::ostream &output_stream;

  bool tryExecuteReturn(const Any& arg,std::ostream &/*error_stream*/) override;
  void executeShow(const Any& value) override;

  FakeExecutor(
    const Environment *parent_environment_ptr,
    std::ostringstream &execution_stream_arg,
    std::ostream &output_stream_arg,
    std::ostream &debug_stream
  )
  : Executor(
      parent_environment_ptr,
      debug_stream
    ),
    execution_stream(execution_stream_arg),
    output_stream(output_stream_arg)
  {
  }
};
