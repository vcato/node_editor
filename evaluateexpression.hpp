#include "optional.hpp"
#include "any.hpp"
#include "parser.hpp"


extern Optional<Any>
  evaluateExpression(
    Parser &parser,
    const std::vector<float> &input_values,
    int &input_index
  );
