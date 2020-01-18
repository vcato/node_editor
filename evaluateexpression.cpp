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
struct Evaluator {
  const ExpressionEvaluatorData &data;
  vector<Any> stack;

  Evaluator(const ExpressionEvaluatorData &data) : data(data) {}

  bool evaluateNumber(const string &text)
  {
    // This could throw an exception.  We should probably catch it and
    // return false.
    Optional<int> maybe_number = maybeInt(text);

    if (!maybe_number) {
      return false;
    }

    push(std::move(*maybe_number));
    return true;
  }

  bool evaluateDollar()
  {
    push(Any(data.input_values[data.input_index]));
    ++data.input_index;
    return true;
  }

  bool evaluateVariable(const string &identifier)
  {
    Optional<Any> maybe_value =
      variableValue(identifier, data.environment_ptr);

    if (!maybe_value) {
      return false;
    }

    push(std::move(*maybe_value));
    return true;
  }

  void makeVector(int n_elements)
  {
    vector<Any> v;

    for (int i=0; i!=n_elements; ++i) {
      v.push_back(std::move(*(stack.end()-n_elements+i)));
    }

    stack.erase(stack.end()-n_elements, stack.end());
    push(Any(std::move(v)));
  }

  void push(Any arg)
  {
    stack.push_back(std::move(arg));
  }

  Any pop()
  {
    Any result = std::move(stack.back());
    stack.pop_back();
    return result;
  }
};
}



namespace {
struct ExpressionEvaluator : ExpressionEvaluatorData {
  Evaluator &evaluator;

  ExpressionEvaluator(const ExpressionEvaluatorData &data, Evaluator &evaluator)
  : ExpressionEvaluatorData(data),
    evaluator(evaluator)
  {
  }

  Optional<Any> evaluateExpression() const;
  bool parseExpression() const;
  bool parsePrimary() const;

  Optional<Any>
    evaluatePrimaryStartingWithIdentifier(const string &) const;

  Optional<Any>
    evaluateFactorStartingWith(Optional<Any> maybe_first_term) const;

  bool parseFactor() const;
  Optional<Any> evaluateStartingWithIdentifier(const string &) const;
  Optional<Any> evaluateTermStartingWith(Optional<Any> maybe_first_term) const;
  Optional<Any> evaluateAddition(const Any &first_term) const;
  Optional<Any> evaluateSubtraction(const Any &first_term) const;
  Optional<Any> evaluateMultiplication(const Any &first_term) const;
  Optional<Any> evaluateDivision(const Any &first_term) const;
  bool parseMember() const;
  Optional<Any> evaluateFunctionCall(const Any &first_term) const;
  bool parsePostfix() const;
  Optional<Any> evaluatePostfixStartingWith(Optional<Any> first_term) const;
  bool parsePostfixStartingWith() const;
  Optional<Any> evaluateObjectConstruction(const Class &) const;
};
}


Optional<Any>
  ExpressionEvaluator::evaluatePrimaryStartingWithIdentifier(
    const string &identifier
  ) const
{
  if (!evaluator.evaluateVariable(identifier)) {
    error_stream << "Unknown name: " << identifier << "\n";
    return {};
  }

  return evaluator.pop();
}


bool ExpressionEvaluator::parsePrimary() const
{
  if (parser.peekChar()=='(') {
    parser.skipChar();

    if (!parseExpression()) {
      return {};
    }

    if (parser.peekChar()!=')') {
      error_stream << "Missing ')'\n";
      return {};
    }

    parser.skipChar();
    return true;
  }

  if (Optional<StringParser::Range> maybe_range = parser.maybeNumberRange()) {
    if (!evaluator.evaluateNumber(parser.rangeText(*maybe_range))) {
      assert(false); // not tested
    }

    return true;
  }

  if (parser.peekChar()=='$') {
    if (!evaluator.evaluateDollar()) {
      assert(false); // not tested
    }

    parser.skipChar();
    return true;
  }

  if (parser.peekChar()=='[') {
    parser.skipChar();

    int n_elements = 0;

    if (parser.peekChar()==']') {
      parser.skipChar();
    }
    else {
      for (;;) {
        Optional<Any> maybe_value = evaluateExpression();

        if (!maybe_value) {
          return false;
        }

        evaluator.push(std::move(*maybe_value));
        ++n_elements;

        if (parser.peekChar()==']') {
          parser.skipChar();
          break;
        }

        if (parser.peekChar()==',') {
          parser.skipChar();
        }
      }
    }

    evaluator.makeVector(n_elements);
    return true;
  }

  Optional<StringParser::Range> maybe_identifier_range =
    parser.maybeIdentifierRange();

  if (maybe_identifier_range) {
    string identifier = parser.rangeText(*maybe_identifier_range);

    Optional<Any> maybe_result =
      evaluatePrimaryStartingWithIdentifier(identifier);

    if (!maybe_result) {
      return false;
    }

    evaluator.push(std::move(*maybe_result));
    return true;
  }

  if (parser.atEnd()) {
    error_stream << "Unexpected end of expression.\n";
    return false;
  }

  error_stream << "Unexpected '" << parser.peekChar() << "'\n";

  return false;
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
    Optional<StringParser::Range> maybe_identifier_range =
      parser.maybeIdentifierRange();

    if (!maybe_identifier_range) {
      assert(false);
    }

    string identifier = parser.rangeText(*maybe_identifier_range);

    if (parser.peekChar() == '=') {
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
  if (!parseFactor()) {
    return {};
  }

  Any second_term = evaluator.pop();

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
  if (!parseFactor()) {
    return {};
  }

  Any second_term = evaluator.pop();

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
  if (!parsePostfix()) {
    return {};
  }

  Any second_term = evaluator.pop();

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
  if (!parsePostfix()) {
    return {};
  }

  Any second_term = evaluator.pop();

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


bool ExpressionEvaluator::parseMember() const
{
  Any first_term = evaluator.pop();

  Optional<StringParser::Range> maybe_identifier_range =
    parser.maybeIdentifierRange();

  if (!maybe_identifier_range) {
    assert(false); // not tested
  }

  string member_name = parser.rangeText(*maybe_identifier_range);

  if (Optional<Point2D> maybe_point2d = maybePoint2D(first_term)) {
    if (member_name=="x") {
      evaluator.push(Any(maybe_point2d->x));
      return true;
    }

    if (member_name=="y") {
      evaluator.push(Any(maybe_point2d->y));
      return true;
    }

    return false;
  }

  if (!first_term.isObject()) {
    error_stream <<
      "Attempt to get member of " << first_term.typeName() << "\n";
    return false;
  }

  Optional<Any> maybe_result = first_term.asObject().maybeMember(member_name);

  if (!maybe_result) {
    return false;
  }

  evaluator.push(std::move(*maybe_result));
  return true;
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


bool ExpressionEvaluator::parsePostfixStartingWith() const
{
  Optional<Any> maybe_first_term;

  for (;;) {
    if (parser.peekChar()=='.') {
      parser.skipChar();

      if (!parseMember()) {
        return false;
      }
    }
    else if (parser.peekChar()=='(') {
      parser.skipChar();
      Optional<Any> maybe_result = evaluateFunctionCall(evaluator.pop());

      if (!maybe_result) {
        return false;
      }

      evaluator.push(std::move(*maybe_result));
    }
    else {
      break;
    }
  }

  return true;
}


Optional<Any>
  ExpressionEvaluator::evaluatePostfixStartingWith(
    Optional<Any> maybe_first_term
  ) const
{
  if (!maybe_first_term) {
    return {};
  }

  evaluator.push(std::move(*maybe_first_term));

  if (!parsePostfixStartingWith()) {
    return {};
  }

  return evaluator.pop();
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


bool ExpressionEvaluator::parsePostfix() const
{
  if (!parsePrimary()) {
    return false;
  }

  return parsePostfixStartingWith();
}


bool ExpressionEvaluator::parseFactor() const
{
  if (!parsePostfix()) {
    return false;
  }

  Optional<Any> maybe_result = evaluateFactorStartingWith(evaluator.pop());

  if (!maybe_result) {
    return false;
  }

  evaluator.push(std::move(*maybe_result));
  return true;
}


bool ExpressionEvaluator::parseExpression() const
{
  if (!parseFactor()) {
    return false;
  }

  Optional<Any> result = evaluateTermStartingWith(evaluator.pop());

  if (!result) {
    return false;
  }

  evaluator.push(std::move(*result));
  return true;
}


Optional<Any> ExpressionEvaluator::evaluateExpression() const
{
  if (!parseExpression()) {
    return {};
  }

  return evaluator.pop();
}


Optional<Any> evaluateExpression(const ExpressionEvaluatorData &data)
{
  Evaluator evaluator(data);

  if (!ExpressionEvaluator(data, evaluator).parseExpression()) {
    return {};
  }

  return evaluator.pop();
}


Optional<Any>
  evaluateExpressionStartingWithIdentifier(
    const ExpressionEvaluatorData &data,
    const string &identifier
  )
{
  Evaluator evaluator(data);
  return
    ExpressionEvaluator(data, evaluator).evaluateStartingWithIdentifier(
      identifier
    );
}
