#include "optional.hpp"
#include "any.hpp"
#include "stringparser.hpp"


extern Optional<Any>
  evaluateExpression(
    StringParser &parser,
    const std::vector<Any> &input_values,
    int &input_index,
    std::ostream &error_stream
  );
