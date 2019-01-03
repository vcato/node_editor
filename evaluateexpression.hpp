#include "optional.hpp"
#include "any.hpp"
#include "stringparser.hpp"
#include "environment.hpp"

struct ExpressionEvaluatorData {
  StringParser &parser;
  const std::vector<Any> &input_values;
  int &input_index;
  std::ostream &error_stream;
  const Environment *environment_ptr;
};


extern Optional<Any> evaluateExpression(const ExpressionEvaluatorData &data);

extern Optional<Any>
  evaluateExpressionStartingWithIdentifier(
    const ExpressionEvaluatorData &data,
    const std::string &identifier
  );
