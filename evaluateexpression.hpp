#include "optional.hpp"
#include "any.hpp"
#include "parser.hpp"


extern Optional<Any>
  evaluateExpression(
    Parser &parser,
    const std::vector<Any> &input_values,
    int &input_index
  );
