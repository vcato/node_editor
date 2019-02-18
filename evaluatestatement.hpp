#include <string>
#include "optional.hpp"
#include "any.hpp"
#include "executor.hpp"

using AllocateEnvironmentFunction =
  std::function<Environment &(const Environment *parent_environment_ptr)>;

extern Optional<Any>
  evaluateStatement(
    const std::string &statement,
    const std::vector<Any> &input_values,
    Executor &executor,
    std::ostream &error_stream,
    const AllocateEnvironmentFunction &
  );
