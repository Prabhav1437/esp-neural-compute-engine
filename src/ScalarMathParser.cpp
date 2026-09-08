#include "ScalarMathParser.h"
#include <math.h>

void MathParser::nextChar() {
  pos++;
  currentChar = (pos < (int)strlen(expr)) ? expr[pos] : '\0';
}

void MathParser::skipWhitespace() {
  while (currentChar != '\0' && (currentChar == ' ' || currentChar == '\t' || currentChar == '\r')) {
    nextChar();
  }
}

double MathParser::evaluate(const char* inputExpr, bool& ok, String& errStr) {
  expr = inputExpr;
  pos = -1;
  ok = true;
  errStr = "";
  nextChar();
  double result = parseExpression(ok, errStr);
  skipWhitespace();
  if (ok && currentChar != '\0') {
    ok = false;
    errStr = "Unexpected trailing characters near '" + String(currentChar) + "'";
  }
  return result;
}

double MathParser::parseFactor(bool& ok, String& errStr) {
  skipWhitespace();
  if (!ok) return 0.0;

  if (currentChar == '+') {
    nextChar();
    return parseFactor(ok, errStr);
  }
  if (currentChar == '-') {
    nextChar();
    return -parseFactor(ok, errStr);
  }

  if (currentChar == '(') {
    nextChar();
    double val = parseExpression(ok, errStr);
    skipWhitespace();
    if (currentChar == ')') {
      nextChar();
    } else {
      ok = false;
      errStr = "Missing closing parenthesis ')'";
      return 0.0;
    }
    return val;
  }

  if ((currentChar >= 'a' && currentChar <= 'z') || (currentChar >= 'A' && currentChar <= 'Z')) {
    int startPos = pos;
    while ((currentChar >= 'a' && currentChar <= 'z') || (currentChar >= 'A' && currentChar <= 'Z')) {
      nextChar();
    }
    String funcName = String(expr).substring(startPos, pos);
    funcName.toLowerCase();
    
    skipWhitespace();
    if (currentChar == '(') {
      nextChar();
      double arg = parseExpression(ok, errStr);
      skipWhitespace();
      if (currentChar == ')') {
        nextChar();
      } else {
        ok = false;
        errStr = "Missing closing parenthesis ')' in function " + funcName;
        return 0.0;
      }

      if (funcName == "sqrt") {
        if (arg < 0) { ok = false; errStr = "Square root of negative number"; return 0.0; }
        return sqrt(arg);
      } else if (funcName == "abs") {
        return fabs(arg);
      } else if (funcName == "sin") {
        return sin(arg);
      } else if (funcName == "cos") {
        return cos(arg);
      } else if (funcName == "tan") {
        return tan(arg);
      } else if (funcName == "log" || funcName == "ln") {
        if (arg <= 0) { ok = false; errStr = "Logarithm of non-positive number"; return 0.0; }
        return log(arg);
      } else if (funcName == "exp") {
        return exp(arg);
      } else {
        ok = false;
        errStr = "Unknown function '" + funcName + "'";
        return 0.0;
      }
    } else {
      ok = false;
      errStr = "Expected '(' after function name '" + funcName + "'";
      return 0.0;
    }
  }

  int startPos = pos;
  if ((currentChar >= '0' && currentChar <= '9') || currentChar == '.') {
    while ((currentChar >= '0' && currentChar <= '9') || currentChar == '.') {
      nextChar();
    }
    if (currentChar == 'e' || currentChar == 'E') {
      nextChar();
      if (currentChar == '+' || currentChar == '-') nextChar();
      while (currentChar >= '0' && currentChar <= '9') nextChar();
    }
    char numBuf[64];
    int len = pos - startPos;
    if (len >= (int)sizeof(numBuf)) len = sizeof(numBuf) - 1;
    strncpy(numBuf, expr + startPos, len);
    numBuf[len] = '\0';
    
    char* endPtr;
    double val = strtod(numBuf, &endPtr);
    if (endPtr == numBuf) {
      ok = false;
      errStr = "Invalid number format near: " + String(numBuf);
      return 0.0;
    }
    return val;
  }

  ok = false;
  if (currentChar == '\0') {
    errStr = "Unexpected end of expression";
  } else {
    errStr = "Unexpected character: '" + String(currentChar) + "'";
  }
  return 0.0;
}

double MathParser::parseExponent(bool& ok, String& errStr) {
  double base = parseFactor(ok, errStr);
  skipWhitespace();
  while (ok && currentChar == '^') {
    nextChar();
    double exponent = parseExponent(ok, errStr);
    base = pow(base, exponent);
    skipWhitespace();
  }
  return base;
}

double MathParser::parseTerm(bool& ok, String& errStr) {
  double left = parseExponent(ok, errStr);
  skipWhitespace();
  while (ok && (currentChar == '*' || currentChar == '/' || currentChar == '%')) {
    char op = currentChar;
    nextChar();
    double right = parseExponent(ok, errStr);
    if (!ok) return 0.0;

    if (op == '*') {
      left *= right;
    } else if (op == '/') {
      if (right == 0.0) {
        ok = false;
        errStr = "Division by zero error";
        return 0.0;
      }
      left /= right;
    } else if (op == '%') {
      if ((long)right == 0) {
        ok = false;
        errStr = "Modulo by zero error";
        return 0.0;
      }
      left = fmod(left, right);
    }
    skipWhitespace();
  }
  return left;
}

double MathParser::parseExpression(bool& ok, String& errStr) {
  double left = parseTerm(ok, errStr);
  skipWhitespace();
  while (ok && (currentChar == '+' || currentChar == '-')) {
    char op = currentChar;
    nextChar();
    double right = parseTerm(ok, errStr);
    if (!ok) return 0.0;

    if (op == '+') {
      left += right;
    } else if (op == '-') {
      left -= right;
    }
    skipWhitespace();
  }
  return left;
}
