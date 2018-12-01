#include "evaluateexpression.hpp"


using std::vector;
using std::cerr;
using std::string;
using std::ostream;
using std::map;


namespace {
struct ExpressionEvaluator : ExpressionEvaluatorData {
  ExpressionEvaluator(const ExpressionEvaluatorData &data)
  : ExpressionEvaluatorData(data)
  {
  }

  Optional<Any> evaluatePrimaryExpression() const;
  Optional<Any>
    evaluatePrimaryExpressionStartingWithIdentifier(const string &) const;
  Optional<Any> evaluateExpression() const;
  Optional<Any> evaluateExpressionStartingWithIdentifier(const string &) const;
  Optional<Any>
    evaluateExpressionStartingWithTerm(
      Optional<Any> maybe_first_term
    ) const;
  Optional<Any> parseObjectConstuction(const Class &) const;
};
}


Optional<Any>
  ExpressionEvaluator::evaluatePrimaryExpressionStartingWithIdentifier(
    const string &identifier
  ) const
{
  Optional<Any> maybe_value = variableValue(identifier,environment);

  if (!maybe_value) {
    error_stream << "Unknown name: " << identifier << "\n";
    return {};
  }

  return maybe_value;
}


Optional<Any> ExpressionEvaluator::evaluatePrimaryExpression() const
{
  float value = 0;

  if (parser.peekChar()=='(') {
    parser.skipChar();
    Optional<Any> result = evaluateExpression();

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
    return evaluateExpressionStartingWithTerm(value);
  }

  if (parser.peekChar()=='[') {
    parser.skipChar();

    std::vector<Any> vector_value;

    if (parser.peekChar()==']') {
      parser.skipChar();
      return Optional<Any>(Any(std::move(vector_value)));
    }

    for (;;) {
      Optional<Any> maybe_value = evaluateExpression();

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

  string identifier;

  if (parser.getIdentifier(identifier)) {
    return evaluateExpressionStartingWithIdentifier(identifier);
  }

  error_stream << "Unexpected '" << parser.peekChar() << "'\n";

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
  ExpressionEvaluator::evaluateExpressionStartingWithIdentifier(
    const string &identifier
  ) const
{
  return
    evaluateExpressionStartingWithTerm(
      evaluatePrimaryExpressionStartingWithIdentifier(identifier)
    );
}


Optional<Any>
  ExpressionEvaluator::parseObjectConstuction(const Class &the_class) const
{
  // Need to extract this to a separate function.
  assert(parser.peekChar()=='(');
  parser.skipChar();
  map<string,Any> named_parameters;

  if (parser.peekChar()==')') {
    assert(the_class.make_object_function);
    Optional<Object> maybe_object =
      the_class.make_object_function(named_parameters);

    if (!maybe_object) {
      // We got an error trying to create the object with the given
      // parameters.  We need a way for the error to be reported.
      return {};
    }

    assert(false);
  }

  for (;;) {
    string identifier;

    if (parser.getIdentifier(identifier)) {
      if (parser.peekChar()=='=') {
        const string &parameter_name = identifier;
        parser.skipChar();
        Optional<Any> value = evaluateExpression();

        if (!value) {
          assert(false);
        }

        named_parameters[parameter_name] = std::move(*value);
      }
      else {
        assert(false);
      }

      if (parser.peekChar()==',') {
        parser.skipChar();
      }
      else if (parser.peekChar()==')') {
        parser.skipChar();
        break;
      }
      else {
        cerr << "parser.peekChar()='" << parser.peekChar() << "'\n";
        assert(false);
      }
    }
    else {
      assert(false);
    }
  }

  assert(the_class.make_object_function);
  Optional<Object> maybe_object =
    the_class.make_object_function(named_parameters);

  if (!maybe_object) {
    cerr << "Failed to make object\n";
    return {};
  }
  else {
    return Any(std::move(*maybe_object));
  }
}


Optional<Any>
  ExpressionEvaluator::evaluateExpressionStartingWithTerm(
    Optional<Any> maybe_first_term
  ) const
{
  if (!maybe_first_term) {
    return maybe_first_term;
  }

  const Any &first_term = *maybe_first_term;

  parser.skipWhitespace();

  if (parser.peekChar()=='(') {
    if (first_term.isClassPtr()) {
      const Class *class_ptr = first_term.asClassPtr();
      assert(class_ptr);

      return parseObjectConstuction(*class_ptr);
    }

    if (first_term.isFunction()) {
      parser.skipChar();
      if (parser.peekChar()==')') {
        vector<Any> arguments;
        Optional<Any> maybe_result = first_term.asFunction()(arguments);

        if (!maybe_result) {
          assert(false);
        }

        // This should be calling evaluateExpressionStartingWithTerm() again
        // so that we can handle the case where the function returns an
        // object.
        return maybe_result;
      }
      else {
        Optional<Any> maybe_first_argument = evaluateExpression();

        if (!maybe_first_argument) {
          assert(false);
        }

        if (parser.peekChar()!=')') {
          assert(false);
        }

        vector<Any> arguments;
        arguments.push_back(*maybe_first_argument);
        parser.skipChar();

        // This should be calling evaluateExpressionStartingWithTerm() again
        // so that we can handle the case where the function returns an
        // object.
        return first_term.asFunction()(arguments);
      }
    }

    assert(false);
  }

  if (parser.peekChar()=='+') {
    parser.skipChar();

    Optional<Any> maybe_second_term = evaluatePrimaryExpression();

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

    Optional<Any> maybe_second_term = evaluatePrimaryExpression();

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

    Optional<Any> maybe_second_term = evaluatePrimaryExpression();

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

  if (parser.peekChar()=='.') {
    parser.skipChar();
    string member_name;
    parser.getIdentifier(member_name);

    if (!first_term.isObject()) {
      assert(false);
    }

    return first_term.asObject().maybeMember(member_name);
  }

  return first_term;
}


Optional<Any> ExpressionEvaluator::evaluateExpression() const
{
  return evaluateExpressionStartingWithTerm(evaluatePrimaryExpression());
}


Optional<Any> evaluateExpression(const ExpressionEvaluatorData &data)
{
  return ExpressionEvaluator(data).evaluateExpression();
}


Optional<Any>
  evaluateExpressionStartingWithIdentifier(
    const ExpressionEvaluatorData &data,
    const string &identifier
  )
{
  return
    ExpressionEvaluator(data).evaluateExpressionStartingWithIdentifier(
      identifier
    );
}
