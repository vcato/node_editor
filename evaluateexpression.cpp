#include "evaluateexpression.hpp"

#include "maybepoint2d.hpp"
#include "anyio.hpp"
#include "maybeint.hpp"


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

  Optional<Any> evaluateExpression() const;
  Optional<Any> evaluatePrimary() const;
  Optional<Any>
    evaluatePrimaryStartingWithIdentifier(const string &) const;
  Optional<Any>
    evaluateFactorStartingWith(Optional<Any> maybe_first_term) const;
  Optional<Any> evaluateFactor() const;
  Optional<Any> evaluateStartingWithIdentifier(const string &) const;
  Optional<Any> evaluateTermStartingWith(Optional<Any> maybe_first_term) const;
  Optional<Any> evaluateAddition(const Any &first_term) const;
  Optional<Any> evaluateSubtraction(const Any &first_term) const;
  Optional<Any> evaluateMultiplication(const Any &first_term) const;
  Optional<Any> evaluateDivision(const Any &first_term) const;
  Optional<Any> evaluateMember(const Any &first_term) const;
  Optional<Any> evaluateFunctionCall(const Any &first_term) const;
  Optional<Any> evaluatePostfix() const;
  Optional<Any> evaluatePostfixStartingWith(Optional<Any> first_term) const;
  Optional<Any> evaluateObjectConstruction(const Class &) const;
};
}


Optional<Any>
  ExpressionEvaluator::evaluatePrimaryStartingWithIdentifier(
    const string &identifier
  ) const
{
  Optional<Any> maybe_value = variableValue(identifier,environment_ptr);

  if (!maybe_value) {
    error_stream << "Unknown name: " << identifier << "\n";
    return {};
  }

  return maybe_value;
}


Optional<Any> ExpressionEvaluator::evaluatePrimary() const
{
  if (parser.peekChar()=='(') {
    parser.skipChar();
    Optional<Any> result = evaluateExpression();

    if (!result) {
      return {};
    }

    if (parser.peekChar()!=')') {
      error_stream << "Missing ')'\n";
      return {};
    }

    parser.skipChar();
    return result;
  }

  if (Optional<StringParser::Range> maybe_range = parser.maybeNumberRange()) {
    // This could throw an exception.  We should probably catch it and
    // return false.
    Optional<int> maybe_number = maybeInt(parser.rangeText(*maybe_range));

    if (!maybe_number) {
      assert(false); // not tested
    }

    return Optional<Any>(Any(*maybe_number));
  }

  if (parser.peekChar()=='$') {
    const Any& value = input_values[input_index];
    ++input_index;
    parser.skipChar();
    return value;
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
    return evaluatePrimaryStartingWithIdentifier(identifier);
  }

  if (parser.atEnd()) {
    error_stream << "Unexpected end of expression.\n";
    return {};
  }

  error_stream << "Unexpected '" << parser.peekChar() << "'\n";

  return Optional<Any>();
}


static Optional<Any>
  maybeScaleVector(
    float first_float,
    const vector<Any> &second_vector,
    ostream &error_stream
  )
{
  vector<Any> result;

  for (auto &x : second_vector) {
    if (!x.isFloat()) {
      error_stream << "Invalid vector for scalar multiplication.\n";
      return {};
    }

    result.push_back(first_float*x.asFloat());
  }

  return {std::move(result)};
}


Optional<Any>
  ExpressionEvaluator::evaluateStartingWithIdentifier(
    const string &identifier
  ) const
{
  Optional<Any> result =
    evaluatePrimaryStartingWithIdentifier(identifier);

  result = evaluatePostfixStartingWith(std::move(result));
  result = evaluateTermStartingWith(std::move(result));
  return result;
}


Optional<Any>
  ExpressionEvaluator::evaluateObjectConstruction(const Class &the_class) const
{
  map<string,Any> named_parameters;

  if (parser.peekChar()==')') {
    Optional<Object> maybe_object =
      the_class.maybeMakeObject(named_parameters,error_stream);

    if (!maybe_object) {
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
          // This is the case where we got an error evaluating the
          // expression used to generate the value for the parameter.
          return {};
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
        error_stream << "Missing ','\n";
        return {};
      }
    }
    else {
      assert(false);
    }
  }

  Optional<Object> maybe_object =
    the_class.maybeMakeObject(named_parameters,error_stream);

  if (!maybe_object) {
    cerr << "Failed to make object\n";
    return {};
  }
  else {
    return Any(std::move(*maybe_object));
  }
}


Optional<Any>
  ExpressionEvaluator::evaluateAddition(const Any &first_term) const
{
  Optional<Any> maybe_second_term = evaluateFactor();

  if (!maybe_second_term) {
    return {};
  }

  const Any &second_term = *maybe_second_term;

  if (first_term.isFloat() && second_term.isFloat()) {
    return {first_term.asFloat() + second_term.asFloat()};
  }

  if (!first_term.isVector() || !second_term.isVector()) {
    string first_term_type_name = first_term.typeName();
    string second_term_type_name = second_term.typeName();
    error_stream << "Invalid types for addition: " <<
      first_term_type_name << " and " << second_term_type_name << ".\n";
    return {};
  }

  const vector<Any> &first_vector = first_term.asVector();
  const vector<Any> &second_vector = second_term.asVector();

  if (first_vector.size() != second_vector.size()) {
    error_stream << "Can't add vectors of different sizes.\n";
    return {};
  }

  auto n = first_vector.size();
  vector<Any> result;

  for (decltype(n) i=0; i!=n; ++i) {
    if (!first_vector[i].isFloat() || !second_vector[i].isFloat()) {
      error_stream << "Can't add vectors containing non-numeric values.\n";
      return {};
    }

    float first_value = first_vector[i].asFloat();
    float second_value = second_vector[i].asFloat();

    result.push_back(first_value + second_value);
  }

  return {std::move(result)};
}


Optional<Any>
  ExpressionEvaluator::evaluateSubtraction(const Any &first_term) const
{
  Optional<Any> maybe_second_term = evaluateFactor();

  if (!maybe_second_term) {
    return {};
  }

  const Any &second_term = *maybe_second_term;

  if (first_term.isFloat() && second_term.isFloat()) {
    return Any(first_term.asFloat() - second_term.asFloat());
  }

  if (!first_term.isVector() || !second_term.isVector()) {
    error_stream << "Invalid types for subtraction.\n";
    return {};
  }

  const vector<Any> &first_vector = first_term.asVector();
  const vector<Any> &second_vector = second_term.asVector();

  if (first_vector.size() != second_vector.size()) {
    error_stream << "Can't subtract vectors of different sizes.\n";
    return {};
  }

  auto n = first_vector.size();
  vector<Any> result;

  for (decltype(n) i=0; i!=n; ++i) {
    if (!first_vector[i].isFloat() || !second_vector[i].isFloat()) {
      error_stream << "Can't subtract vectors containing non-numeric values.\n";
      return {};
    }

    float first_value = first_vector[i].asFloat();
    float second_value = second_vector[i].asFloat();

    result.push_back(first_value - second_value);
  }

  return {std::move(result)};
}


Optional<Any>
  ExpressionEvaluator::evaluateMultiplication(const Any &first_term) const
{
  Optional<Any> maybe_second_term = evaluatePostfix();

  if (!maybe_second_term) {
    return {};
  }

  const Any &second_term = *maybe_second_term;

  if (first_term.isFloat() && second_term.isFloat()) {
    float first_float = first_term.asFloat();
    float second_float = second_term.asFloat();
    return {first_float * second_float};
  }

  if (first_term.isFloat() && second_term.isVector()) {
    return
      maybeScaleVector(
        first_term.asFloat(),
        second_term.asVector(),
        error_stream
      );
  }

  if (first_term.isVector() && second_term.isFloat()) {
    return
      maybeScaleVector(
        second_term.asFloat(),
        first_term.asVector(),
        error_stream
      );
  }

  error_stream << "Unhandled multiplication: " <<
    first_term.typeName() << "*" <<
    second_term.typeName() << "\n";
  return {};
}


Optional<Any>
  ExpressionEvaluator::evaluateDivision(const Any &first_term) const
{
  Optional<Any> maybe_second_term = evaluatePostfix();

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
        error_stream <<
          "Can't divide a vector containing non-numeric values.\n";
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


Optional<Any>
  ExpressionEvaluator::evaluateMember(const Any &first_term) const
{
  string member_name;
  parser.getIdentifier(member_name);

  if (Optional<Point2D> maybe_point2d = maybePoint2D(first_term)) {
    if (member_name=="x") {
      return Any(maybe_point2d->x);
    }

    if (member_name=="y") {
      return Any(maybe_point2d->y);
    }

    return {};
  }

  if (!first_term.isObject()) {
    error_stream <<
      "Attempt to get member of " << first_term.typeName() << "\n";
    return {};
  }

  return first_term.asObject().maybeMember(member_name);
}


Optional<Any>
  ExpressionEvaluator::evaluateFunctionCall(const Any &first_term) const
{
  if (first_term.isClassPtr()) {
    const Class *class_ptr = first_term.asClassPtr();
    assert(class_ptr);

    return evaluateObjectConstruction(*class_ptr);
  }

  if (first_term.isFunction()) {
    if (parser.peekChar()==')') {
      vector<Any> arguments;
      Optional<Any> maybe_result = first_term.asFunction()(arguments);

      if (!maybe_result) {
        assert(false);
      }

      return maybe_result;
    }
    else {
      Optional<Any> maybe_first_argument = evaluateExpression();

      if (!maybe_first_argument) {
        return {};
      }

      if (parser.peekChar()!=')') {
        assert(false);
      }

      vector<Any> arguments;
      arguments.push_back(*maybe_first_argument);
      parser.skipChar();

      return first_term.asFunction()(arguments);
    }
  }

  // We had [term '('], but term wasn't a class or a function.
  // What type did it have?
  cerr << "first_term.typeName():" << first_term.typeName() << "\n";
  assert(false);
}


Optional<Any>
  ExpressionEvaluator::evaluatePostfixStartingWith(
    Optional<Any> maybe_first_term
  ) const
{
  while (maybe_first_term) {
    const Any &first_term = *maybe_first_term;

    if (parser.peekChar()=='.') {
      parser.skipChar();
      maybe_first_term = evaluateMember(first_term);
    }
    else if (parser.peekChar()=='(') {
      parser.skipChar();
      maybe_first_term = evaluateFunctionCall(first_term);
    }
    else {
      break;
    }
  }

  return maybe_first_term;
}


Optional<Any>
  ExpressionEvaluator::evaluateFactorStartingWith(
    Optional<Any> maybe_first_factor
  ) const
{
  while (maybe_first_factor) {
    const Any &first_factor = *maybe_first_factor;

    if (parser.peekChar()=='*') {
      parser.skipChar();
      maybe_first_factor = evaluateMultiplication(first_factor);
    }
    else if (parser.peekChar()=='/') {
      parser.skipChar();
      maybe_first_factor = evaluateDivision(first_factor);
    }
    else {
      break;
    }
  }

  return maybe_first_factor;
}


Optional<Any>
  ExpressionEvaluator::evaluateTermStartingWith(
    Optional<Any> maybe_first_term
  ) const
{
  while (maybe_first_term) {
    const Any &first_term = *maybe_first_term;

    parser.skipWhitespace();

    if (parser.peekChar()=='+') {
      parser.skipChar();
      maybe_first_term = evaluateAddition(first_term);
    }
    else if (parser.peekChar()=='-') {
      parser.skipChar();
      maybe_first_term = evaluateSubtraction(first_term);
    }
    else {
      break;
    }
  }

  return maybe_first_term;
}


Optional<Any> ExpressionEvaluator::evaluatePostfix() const
{
  return evaluatePostfixStartingWith(evaluatePrimary());
}


Optional<Any> ExpressionEvaluator::evaluateFactor() const
{
  return evaluateFactorStartingWith(evaluatePostfix());
}


Optional<Any> ExpressionEvaluator::evaluateExpression() const
{
  return evaluateTermStartingWith(evaluateFactor());
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
    ExpressionEvaluator(data).evaluateStartingWithIdentifier(
      identifier
    );
}
