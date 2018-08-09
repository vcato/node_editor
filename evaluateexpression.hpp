#include "optional.hpp"
#include "any.hpp"
#include "stringparser.hpp"
#include "environment.hpp"


extern Optional<Any>
  evaluateExpression(
    StringParser &parser,
    const std::vector<Any> &input_values,
    int &input_index,
    std::ostream &error_stream,
    const Environment &
  );

extern Optional<Any>
  evaluateExpressionStartingWithIdentifier(
    const std::string &identifier,
    StringParser &parser,
    const std::vector<Any> &input_values,
    int &input_index,
    std::ostream &error_stream,
    const Environment &environment
  );
