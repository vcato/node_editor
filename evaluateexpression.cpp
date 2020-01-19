#include "evaluateexpression.hpp"

#include "maybepoint2d.hpp"
#include "anyio.hpp"
#include "maybeint.hpp"
#include "expressionparser.hpp"

using std::vector;
using std::cerr;
using std::string;
using std::ostream;
using std::map;

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


namespace {
struct Evaluator : EvaluatorInterface {
  const ExpressionEvaluatorData &data;
  vector<Any> stack;

  Evaluator(const ExpressionEvaluatorData &data) : data(data) {}

  string rangeText(const StringRange &range)
  {
    return data.parser.rangeText(range);
  }

  bool evaluateNumber(const StringRange &number_range)
  {
    const string &text = rangeText(number_range);

    // This could throw an exception.  We should probably catch it and
    // return false.
    Optional<int> maybe_number = maybeInt(text);

    if (!maybe_number) {
      return false;
    }

    push(std::move(*maybe_number));
    return true;
  }

  bool evaluateDollar() override
  {
    push(Any(data.input_values[data.input_index]));
    ++data.input_index;
    return true;
  }

  bool evaluateVariable(const StringRange &identifer_range) override
  {
    const string &identifier = rangeText(identifer_range);

    Optional<Any> maybe_value =
      variableValue(identifier, data.environment_ptr);

    if (!maybe_value) {
      data.error_stream << "Unknown name: " << identifier << "\n";
      return false;
    }

    push(std::move(*maybe_value));
    return true;
  }

  bool evaluateMember(const StringRange &name_range) override
  {
    const string &member_name = rangeText(name_range);
    Evaluator &evaluator = *this;
    ostream &error_stream = data.error_stream;
    Any first_term = evaluator.pop();

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

  bool evaluateAddition() override
  {
    Evaluator &evaluator = *this;
    ostream &error_stream = data.error_stream;
    Any second_term = evaluator.pop();
    Any first_term = evaluator.pop();

    if (first_term.isFloat() && second_term.isFloat()) {
      evaluator.push( {first_term.asFloat() + second_term.asFloat()});
      return true;
    }

    if (!first_term.isVector() || !second_term.isVector()) {
      string first_term_type_name = first_term.typeName();
      string second_term_type_name = second_term.typeName();
      error_stream << "Invalid types for addition: " <<
        first_term_type_name << " and " << second_term_type_name << ".\n";
      return false;
    }

    const vector<Any> &first_vector = first_term.asVector();
    const vector<Any> &second_vector = second_term.asVector();

    if (first_vector.size() != second_vector.size()) {
      error_stream << "Can't add vectors of different sizes.\n";
      return false;
    }

    auto n = first_vector.size();
    vector<Any> result;

    for (decltype(n) i=0; i!=n; ++i) {
      if (!first_vector[i].isFloat() || !second_vector[i].isFloat()) {
        error_stream << "Can't add vectors containing non-numeric values.\n";
        return false;
      }

      float first_value = first_vector[i].asFloat();
      float second_value = second_vector[i].asFloat();

      result.push_back(first_value + second_value);
    }

    evaluator.push( {std::move(result)} );
    return true;
  }

  bool evaluateSubtraction() override
  {
    Evaluator &evaluator = *this;
    ostream &error_stream = data.error_stream;
    Any second_term = evaluator.pop();
    Any first_term = evaluator.pop();

    if (first_term.isFloat() && second_term.isFloat()) {
      evaluator.push(first_term.asFloat() - second_term.asFloat());
      return true;
    }

    if (!first_term.isVector() || !second_term.isVector()) {
      error_stream << "Invalid types for subtraction.\n";
      return false;
    }

    const vector<Any> &first_vector = first_term.asVector();
    const vector<Any> &second_vector = second_term.asVector();

    if (first_vector.size() != second_vector.size()) {
      error_stream << "Can't subtract vectors of different sizes.\n";
      return false;
    }

    auto n = first_vector.size();
    vector<Any> result;

    for (decltype(n) i=0; i!=n; ++i) {
      if (!first_vector[i].isFloat() || !second_vector[i].isFloat()) {
        error_stream << "Can't subtract vectors containing non-numeric values.\n";
        return false;
      }

      float first_value = first_vector[i].asFloat();
      float second_value = second_vector[i].asFloat();

      result.push_back(first_value - second_value);
    }

    evaluator.push(std::move(result));
    return true;
  }

  bool evaluateMultiplication() override
  {
    Evaluator &evaluator = *this;
    ostream &error_stream = data.error_stream;
    Any second_term = evaluator.pop();
    Any first_term = evaluator.pop();

    if (first_term.isFloat() && second_term.isFloat()) {
      float first_float = first_term.asFloat();
      float second_float = second_term.asFloat();
      evaluator.push( {first_float * second_float} );
      return true;
    }

    if (first_term.isFloat() && second_term.isVector()) {
      Optional<Any> maybe_result =
        maybeScaleVector(
          first_term.asFloat(),
          second_term.asVector(),
          error_stream
        );

      if (!maybe_result) {
        return false;
      }

      evaluator.push(std::move(*maybe_result));
      return true;
    }

    if (first_term.isVector() && second_term.isFloat()) {
      Optional<Any> maybe_result =
        maybeScaleVector(
          second_term.asFloat(),
          first_term.asVector(),
          error_stream
        );

      if (!maybe_result) {
        return false;
      }

      evaluator.push(std::move(*maybe_result));
      return true;
    }

    error_stream << "Unhandled multiplication: " <<
      first_term.typeName() << "*" <<
      second_term.typeName() << "\n";

    return false;
  }

  bool evaluateDivision() override
  {
    Evaluator &evaluator = *this;
    ostream &error_stream = data.error_stream;
    Any second_term = evaluator.pop();
    Any first_term = evaluator.pop();

    if (first_term.isVector() && second_term.isFloat()) {
      const vector<Any> &first_vector = first_term.asVector();
      float second_float = second_term.asFloat();
      vector<Any> result;

      for (auto &x : first_vector) {
        if (!x.isFloat()) {
          error_stream <<
            "Can't divide a vector containing non-numeric values.\n";
          return false;
        }

        result.push_back(x.asFloat() / second_float);
      }

      evaluator.push(std::move(result));
      return true;
    }
    else if (first_term.isFloat() && second_term.isFloat()) {
      evaluator.push( {first_term.asFloat() / second_term.asFloat()} );
      return true;
    }
    else {
      error_stream << "Unknown operation: " << first_term.typeName() << "/" <<
        second_term.typeName() << "\n";
    }

    return false;
  }

  bool
  evaluateObjectConstruction(
    const Class &the_class, int n_arguments, int n_extra
  )
  {
    Evaluator &evaluator = *this;
    ostream &error_stream = data.error_stream;
    vector<Any> arguments;
    map<string,Any> named_arguments;
    auto iter = evaluator.stack.end() - n_arguments*2;

    for (int i=0; i!=n_arguments; ++i) {
      string parameter_name = std::move(*iter++).asString();
      named_arguments[parameter_name] = std::move(*iter++);
    }

    evaluator.stack.erase(evaluator.stack.end() - n_arguments*2 - n_extra, evaluator.stack.end());

    assert(arguments.empty());
    Optional<Object> maybe_object =
      the_class.maybeMakeObject(named_arguments,error_stream);

    if (!maybe_object) {
      return false;
    }

    evaluator.push(Any(std::move(*maybe_object)));
    return true;
  }

  bool
  evaluateFunctionCall(
    const Any &first_term, const int n_arguments, const int n_extra
  )
  {
    Evaluator &evaluator = *this;
    vector<Any> arguments;
    map<string,Any> named_arguments;

    {
      auto iter = evaluator.stack.end() - n_arguments*2;

      for (int i=0; i!=n_arguments; ++i) {
        if (iter->isVoid()) {
          ++iter;
          arguments.push_back(std::move(*iter++));
        }
        else {
          string parameter_name = std::move(*iter++).asString();
          named_arguments[parameter_name] = std::move(*iter++);
        }
      }

      evaluator.stack.erase(evaluator.stack.end() - n_arguments*2 - n_extra, evaluator.stack.end());
    }

    Optional<Any> maybe_result = first_term.asFunction()(arguments);

    if (!maybe_result) {
      return false;
    }

    evaluator.push(std::move(*maybe_result));
    return true;
  }

  bool evaluateCall(const int n_arguments) override
  {
    Evaluator &evaluator = *this;
    ostream &error_stream = data.error_stream;
    Any &first_term = *(evaluator.stack.end() - n_arguments*2 - 1);

    if (first_term.isClassPtr()) {
      const Class *class_ptr = first_term.asClassPtr();
      assert(class_ptr);
      const Class &the_class = *class_ptr;

      return
        evaluator.evaluateObjectConstruction(
          the_class, n_arguments, /*n_extra*/1
        );
    }
    else if (first_term.isFunction()) {
      return
        evaluator.evaluateFunctionCall(first_term, n_arguments, /*n_extra*/1);
    }

    // We had [term '('], but term wasn't a class or a function.
    // What type did it have?
    error_stream << "first_term.typeName():" << first_term.typeName() << "\n";
    assert(false);
  }

  void makeVector(int n_elements) override
  {
    vector<Any> v;

    for (int i=0; i!=n_elements; ++i) {
      v.push_back(std::move(*(stack.end()-n_elements+i)));
    }

    stack.erase(stack.end()-n_elements, stack.end());
    push(Any(std::move(v)));
  }

  void evaluateNoName() override
  {
    push(Any()); // name for unnamed argument
  }

  void evaluateName(const StringRange &range) override
  {
    push(rangeText(range));
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


Optional<Any> evaluateExpression(const ExpressionEvaluatorData &data)
{
  Evaluator evaluator(data);

  bool could_parse =
    ExpressionParser(
      data.error_stream, data.parser, evaluator
    ).parseExpression();

  if (!could_parse) {
    return {};
  }

  return evaluator.pop();
}


Optional<Any>
  evaluateExpressionStartingWithIdentifier(
    const ExpressionEvaluatorData &data,
    const StringRange &identifier_range
  )
{
  Evaluator evaluator(data);
  const string &identifier = data.parser.rangeText(identifier_range);

  bool could_parse =
    ExpressionParser(
      data.error_stream, data.parser, evaluator
    ).parseStartingWithIdentifier(identifier_range);

  if (!could_parse) {
    return {};
  }

  return evaluator.pop();
}
