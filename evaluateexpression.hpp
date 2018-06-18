#include "optional.hpp"
#include "any.hpp"
#include "parser.hpp"


extern Optional<Any>
  evaluateExpression(
    Parser &parser,
#if 1
    const std::vector<float> &input_values,
#else
    const std::vector<Any> &input_values,
#endif
    int &input_index
  );
