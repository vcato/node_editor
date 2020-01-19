#include "expressionparser.hpp"

using std::string;


void ExpressionParser::skipChar() const
{
  string_parser.skipChar();
}


char ExpressionParser::peekChar() const
{
  return string_parser.peekChar();
}


bool
  ExpressionParser::parsePrimaryStartingWithIdentifier(
    const StringRange &identifier_range
  ) const
{
  if (!evaluator.evaluateVariable(identifier_range)) {
    return false;
  }

  return true;
}


bool ExpressionParser::parsePrimary() const
{
  if (peekChar()=='(') {
    skipChar();

    if (!parseExpression()) {
      return {};
    }

    if (peekChar()!=')') {
      error_stream << "Missing ')'\n";
      return {};
    }

    skipChar();
    return true;
  }

  if (Optional<StringParser::Range> maybe_range = string_parser.maybeNumberRange()) {
    if (!evaluator.evaluateNumber(*maybe_range)) {
      assert(false); // not tested
    }

    return true;
  }

  if (peekChar()=='$') {
    if (!evaluator.evaluateDollar()) {
      assert(false); // not tested
    }

    skipChar();
    return true;
  }

  if (peekChar()=='[') {
    skipChar();

    int n_elements = 0;

    if (peekChar()==']') {
      skipChar();
    }
    else {
      for (;;) {
        if (!parseExpression()) {
          return false;
        }

        ++n_elements;

        if (peekChar()==']') {
          skipChar();
          break;
        }

        if (peekChar()==',') {
          skipChar();
        }
      }
    }

    evaluator.makeVector(n_elements);
    return true;
  }

  Optional<StringParser::Range> maybe_identifier_range =
    string_parser.maybeIdentifierRange();

  if (maybe_identifier_range) {
    return parsePrimaryStartingWithIdentifier(*maybe_identifier_range);
  }

  if (string_parser.atEnd()) {
    error_stream << "Unexpected end of expression.\n";
    return false;
  }

  error_stream << "Unexpected '" << peekChar() << "'\n";

  return false;
}


bool
  ExpressionParser::parseStartingWithIdentifier(
    const StringRange &identifier_range
  ) const
{
  if (!parsePrimaryStartingWithIdentifier(identifier_range)) {
    return false;
  }

  if (!extendPostfix()) {
    return false;
  }

  if (!extendTerm()) {
    return false;
  }

  return true;
}


bool ExpressionParser::parseFunctionArgument() const
{
  Optional<StringParser::Range> maybe_identifier_range =
    string_parser.maybeIdentifierRange();

  if (!maybe_identifier_range) {
    evaluator.evaluateNoName();

    if (!parseExpression()) {
      return false;
    }
  }
  else {
    if (peekChar() == '=') {
      skipChar();
      evaluator.evaluateName(*maybe_identifier_range);

      if (!parseExpression()) {
        return false;
      }
    }
    else {
      evaluator.evaluateNoName();

      if (!parseStartingWithIdentifier(*maybe_identifier_range)) {
        return false;
      }
    }
  }

  return true;
}


bool ExpressionParser::parseFunctionArguments(int &n_arguments) const
{
  if (peekChar()==')') {
  }
  else {
    for (;;) {
      if (!parseFunctionArgument()) {
        return false;
      }

      ++n_arguments;

      if (peekChar() == ',') {
        skipChar();
      }
      else if (peekChar() == ')') {
        skipChar();
        break;
      }
      else {
        error_stream << "Missing ','\n";
        return false;
      }
    }
  }

  return true;
}


bool ExpressionParser::parseFunctionCall() const
{
  int n_arguments = 0;

  if (!parseFunctionArguments(n_arguments)) {
    return false;
  }

  return evaluator.evaluateCall(n_arguments);
}


bool ExpressionParser::extendPostfix() const
{
  for (;;) {
    if (peekChar()=='.') {
      skipChar();

      Optional<StringParser::Range> maybe_identifier_range =
        string_parser.maybeIdentifierRange();

      if (!maybe_identifier_range) {
        assert(false); // not tested
      }

      if (!evaluator.evaluateMember(*maybe_identifier_range)) {
        return false;
      }
    }
    else if (peekChar()=='(') {
      skipChar();

      if (!parseFunctionCall()) {
        return false;
      }
    }
    else {
      break;
    }
  }

  return true;
}


bool ExpressionParser::extendTerm() const
{
  for (;;) {
    string_parser.skipWhitespace();

    if (peekChar()=='+') {
      skipChar();

      if (!parseFactor()) {
        return false;
      }

      if (!evaluator.evaluateAddition()) {
        return false;
      }
    }
    else if (peekChar()=='-') {
      skipChar();

      if (!parseFactor()) {
        return false;
      }

      if (!evaluator.evaluateSubtraction()) {
        return false;
      }
    }
    else {
      break;
    }
  }

  return true;
}


bool ExpressionParser::parsePostfix() const
{
  if (!parsePrimary()) {
    return false;
  }

  return extendPostfix();
}


bool ExpressionParser::extendFactor() const
{
  for (;;) {
    if (peekChar()=='*') {
      skipChar();

      if (!parsePostfix()) {
        return false;
      }

      if (!evaluator.evaluateMultiplication()) {
        return false;
      }
    }
    else if (peekChar()=='/') {
      skipChar();

      if (!parsePostfix()) {
        return false;
      }

      if (!evaluator.evaluateDivision()) {
        return false;
      }
    }
    else {
      break;
    }
  }

  return true;
}


bool ExpressionParser::parseFactor() const
{
  if (!parsePostfix()) {
    return false;
  }

  return extendFactor();
}


bool ExpressionParser::parseExpression() const
{
  if (!parseFactor()) {
    return false;
  }

  if (!extendTerm()) {
    return false;
  }

  return true;
}
