#ifndef SCALAR_MATH_PARSER_H
#define SCALAR_MATH_PARSER_H

#include <Arduino.h>

class MathParser {
private:
  const char* expr;
  int pos;
  char currentChar;

  void nextChar();
  void skipWhitespace();
  double parseExpression(bool& ok, String& errStr);
  double parseTerm(bool& ok, String& errStr);
  double parseExponent(bool& ok, String& errStr);
  double parseFactor(bool& ok, String& errStr);

public:
  double evaluate(const char* inputExpr, bool& ok, String& errStr);
};

#endif // SCALAR_MATH_PARSER_H
