#include "evaluateexpression.hpp"


Optional<Any>
  evaluateExpression(
    Parser &parser,
    const std::vector<float> &input_values,
    int &input_index
  )
{
  float value = 0;

  if (parser.getNumber(value)) {
    return Optional<Any>(Any(value));
  }

  if (parser.peek()=='$') {
    float value = input_values[input_index];
    ++input_index;
    ++parser.index;
    return Optional<Any>(value);
  }

  if (parser.peek()=='[') {
    parser.skipChar();

    std::vector<Any> vector_value;

    if (parser.peek()==']') {
      assert(false);
    }

    for (;;) {
      Optional<Any> maybe_value =
        evaluateExpression(parser,input_values,input_index);

      if (!maybe_value) {
        assert(false);
      }

      vector_value.push_back(std::move(*maybe_value));

      if (parser.peek()==']') {
        parser.skipChar();
        break;
      }

      if (parser.peek()==',') {
        parser.skipChar();
      }
    }

    return Optional<Any>(std::move(vector_value));

    assert(false);
  }

  return Optional<Any>();
}
