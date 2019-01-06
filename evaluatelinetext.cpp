#include "evaluatelinetext.hpp"

#include "stringparser.hpp"
#include "evaluateexpression.hpp"


using std::string;
using std::vector;
using std::ostream;


Optional<Any>
  evaluateLineText(
    const string &line_text_arg,
    const vector<Any> &input_values,
    Executor &executor,
    ostream &error_stream
  )
{
  int character_index = 0;
  StringParser parser{line_text_arg,character_index};
  parser.skipWhitespace();
  int input_index = 0;

  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    executor.parent_environment_ptr
  };

  string identifier;

  if (parser.getIdentifier(identifier)) {
    if (identifier=="show") {
      if (parser.peekChar()!='(') {
        error_stream << "Missing '(' after show\n";
        return {};
      }

      parser.skipChar();

      Optional<Any> maybe_value = evaluateExpression(data);

      if (!maybe_value) {
        return {};
      }

      if (parser.peekChar()!=')') {
        error_stream << "Missing ')' for show\n";
        return {};
      }

      parser.skipChar();

      executor.executeShow(*maybe_value);

      if (!parser.atEnd()) {
        return {};
      }

      return Any();
    }

    if (identifier=="return") {
      Optional<Any> maybe_value = evaluateExpression(data);

      if (!maybe_value) {
        return {};
      }

      if (!executor.tryExecuteReturn(*maybe_value,error_stream)) {
        return Any();
      }

      return Any();
    }

    return evaluateExpressionStartingWithIdentifier(data,identifier);
  }

  return evaluateExpression(data);
}
