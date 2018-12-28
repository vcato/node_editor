#include <string>
#include "optional.hpp"
#include "any.hpp"
#include "executor.hpp"


extern Optional<Any>
  evaluateLineText(
    const std::string &line_text_arg,
    const std::vector<Any> &input_values,
    Executor &executor,
    std::ostream &error_stream
  );
