#include "evaluateexpression.hpp"


using std::vector;
using std::cerr;


static Optional<Any>
  evaluatePrimaryExpression(
    Parser &parser,
#if 1
    const std::vector<float> &input_values,
#else
    const std::vector<Any> &input_values,
#endif
    int &input_index
  )
{
  float value = 0;

  if (parser.peekChar()=='(') {
    parser.skipChar();
    Optional<Any> result =
      evaluateExpression(parser,input_values,input_index);

    if (!result) {
      return {};
    }

    if (parser.peekChar()!=')') {
      return {};
    }

    parser.skipChar();
    return result;
  }

  if (parser.getNumber(value)) {
    return Optional<Any>(Any(value));
  }

  if (parser.peekChar()=='$') {
    const Any& value = input_values[input_index];
    ++input_index;
    parser.skipChar();
    return Optional<Any>(value);
  }

  if (parser.peekChar()=='[') {
    parser.skipChar();

    std::vector<Any> vector_value;

    if (parser.peekChar()==']') {
      parser.skipChar();
      return Optional<Any>(Any(std::move(vector_value)));
    }

    for (;;) {
      Optional<Any> maybe_value =
        evaluateExpression(parser,input_values,input_index);

      if (!maybe_value) {
        return {};
      }

      vector_value.push_back(std::move(*maybe_value));

      if (parser.peekChar()==']') {
        parser.skipChar();
        break;
      }

      if (parser.peekChar()==',') {
        parser.skipChar();
      }
    }

    return Optional<Any>(std::move(vector_value));
  }

  return Optional<Any>();
}


Optional<Any>
  evaluateExpression(
    Parser &parser,
#if 1
    const std::vector<float> &input_values,
#else
    const std::vector<Any> &input_values,
#endif
    int &input_index
  )
{
  Optional<Any> maybe_first_term =
    evaluatePrimaryExpression(parser,input_values,input_index);

  if (!maybe_first_term) {
    return maybe_first_term;
  }

  const Any &first_term = *maybe_first_term;

  parser.skipWhitespace();

  if (parser.peekChar()=='+') {
    parser.skipChar();

    Optional<Any> maybe_second_term =
      evaluatePrimaryExpression(parser,input_values,input_index);

    if (!maybe_second_term) {
      return {};
    }

    const Any &second_term = *maybe_second_term;

    if (first_term.isFloat() && second_term.isFloat()) {
      float first_float = first_term.asFloat();
      float second_float = second_term.asFloat();
      return Any(first_float + second_float);
    }

    if (!first_term.isVector() || !second_term.isVector()) {
      return {};
    }

    const vector<Any> &first_vector = first_term.asVector();
    const vector<Any> &second_vector = second_term.asVector();

    if (first_vector.size()!=second_vector.size()) {
      return {};
    }

    auto n = first_vector.size();
    vector<Any> result;

    for (decltype(n) i=0; i!=n; ++i) {
      if (!first_vector[i].isFloat()) {
        return {};
      }

      if (!second_vector[i].isFloat()) {
        return {};
      }

      float first_value = first_vector[i].asFloat();
      float second_value = second_vector[i].asFloat();

      result.push_back(first_value + second_value);
    }

    return Any(std::move(result));
  }

  if (parser.peekChar()=='/') {
    parser.skipChar();

    Optional<Any> maybe_second_term =
      evaluatePrimaryExpression(parser,input_values,input_index);

    if (!maybe_second_term) {
      return {};
    }

    const Any &second_term = *maybe_second_term;

    if (first_term.isVector() && second_term.isFloat()) {
      const vector<Any> &first_vector = first_term.asVector();
      float second_float = second_term.asFloat();
      vector<Any> result;

      for (auto &x : first_vector) {
        if (!x.isFloat()) {
          return {};
        }

        result.push_back(x.asFloat() / second_float);
      }

      return Any(std::move(result));
    }

    return {};
  }

  return maybe_first_term;
}
