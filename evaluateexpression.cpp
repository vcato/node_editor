#include "evaluateexpression.hpp"


using std::vector;
using std::cerr;
using std::string;
using std::ostream;


static Optional<Any>
  evaluatePrimaryExpression(
    StringParser &parser,
    const std::vector<Any> &input_values,
    int &input_index,
    ostream &error_stream
  )
{
  float value = 0;

  if (parser.peekChar()=='(') {
    parser.skipChar();
    Optional<Any> result =
      evaluateExpression(
        parser,input_values,input_index,error_stream
      );

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
        evaluateExpression(parser,input_values,input_index,error_stream);

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


static Optional<Any>
  maybeScaleVector(float first_float,const vector<Any> &second_vector)
{
  vector<Any> result;

  for (auto &x : second_vector) {
    if (!x.isFloat()) {
      return {};
    }

    result.push_back(first_float*x.asFloat());
  }

  return {std::move(result)};
}


Optional<Any>
  evaluateExpression(
    StringParser &parser,
    const std::vector<Any> &input_values,
    int &input_index,
    ostream &error_stream
  )
{
  Optional<Any> maybe_first_term =
    evaluatePrimaryExpression(parser,input_values,input_index,error_stream);

  if (!maybe_first_term) {
    return maybe_first_term;
  }

  const Any &first_term = *maybe_first_term;

  parser.skipWhitespace();

  if (parser.peekChar()=='+') {
    parser.skipChar();

    Optional<Any> maybe_second_term =
      evaluatePrimaryExpression(parser,input_values,input_index,error_stream);

    if (!maybe_second_term) {
      return {};
    }

    const Any &second_term = *maybe_second_term;

    if (first_term.isFloat() && second_term.isFloat()) {
      return {first_term.asFloat() + second_term.asFloat()};
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

    return {std::move(result)};
  }

  if (parser.peekChar()=='*') {
    parser.skipChar();

    Optional<Any> maybe_second_term =
      evaluatePrimaryExpression(parser,input_values,input_index,error_stream);

    if (!maybe_second_term) {
      return {};
    }

    const Any &second_term = *maybe_second_term;

    if (first_term.isFloat() && second_term.isFloat()) {
      float first_float = first_term.asFloat();
      float second_float = second_term.asFloat();
      return {first_float * second_float};
    }

    if (first_term.isVector() && second_term.isVector()) {
      return {};
    }

    if (first_term.isFloat() && second_term.isVector()) {
      return maybeScaleVector(first_term.asFloat(),second_term.asVector());
    }

    if (first_term.isVector() && second_term.isFloat()) {
      return maybeScaleVector(second_term.asFloat(),first_term.asVector());
    }

    error_stream << "Unhandled multiplication: " <<
      first_term.typeName() << "*" <<
      second_term.typeName() << "\n";
    return {};
  }

  if (parser.peekChar()=='/') {
    parser.skipChar();

    Optional<Any> maybe_second_term =
      evaluatePrimaryExpression(parser,input_values,input_index,error_stream);

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

      return {std::move(result)};
    }
    else if (first_term.isFloat() && second_term.isFloat()) {
      return {first_term.asFloat() / second_term.asFloat()};
    }
    else {
      error_stream << "Unknown operation: " << first_term.typeName() << "/" <<
        second_term.typeName() << "\n";
    }

    return {};
  }

  return maybe_first_term;
}
