#include "evaluatestatement.hpp"

#include "stringparser.hpp"
#include "evaluateexpression.hpp"
#include "anyio.hpp"

using std::string;
using std::vector;
using std::ostream;
using std::cerr;


Optional<Any>
  evaluateStatement(
    const string &statement,
    const vector<Any> &input_values,
    Executor &executor,
    ostream &error_stream,
    const AllocateEnvironmentFunction &allocate_environment_function
  )
{
  int character_index = 0;
  StringParser parser{statement,character_index};
  parser.skipWhitespace();
  int input_index = 0;

  ExpressionEvaluatorData data{
    parser,
    input_values,
    input_index,
    error_stream,
    executor.parent_environment_ptr
  };

  Optional<StringParser::Range> maybe_identifier_range =
    parser.maybeIdentifierRange();

  if (maybe_identifier_range) {
    string identifier = parser.rangeText(*maybe_identifier_range);

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

    if (parser.peekChar() == '=') {
      const string &variable_name = identifier;
      parser.skipChar();
      Optional<Any> maybe_result = evaluateExpression(data);

      if (maybe_result) {
        Environment &env =
          allocate_environment_function(executor.parent_environment_ptr);
        env[variable_name] = *maybe_result;

        // We have to make the newly allocated environment be the current
        // environment here.
        executor.parent_environment_ptr = &env;
      }
      else {
        // We got an error evaluating the expression, so we can't do
        // the assignment.
      }

      return Any();
    }

    return evaluateExpressionStartingWithIdentifier(data,identifier);
  }

  return evaluateExpression(data);
}
