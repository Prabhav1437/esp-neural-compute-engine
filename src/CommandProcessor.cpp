#include "CommandProcessor.h"
#include "MatrixEngine.h"
#include "ScalarMathParser.h"
#include <math.h>

static String trimStr(String str) {
  str.trim();
  return str;
}

void processCommand(String input) {
  input = trimStr(input);
  if (input.length() == 0) return;

  unsigned long startTime = micros();
  digitalWrite(LED_PIN, HIGH);

  Serial.println("\n--------------------------------------------------");
  Serial.printf("[INPUT RECEIVED]  \"%s\"\n", input.c_str());

  String lowerInput = input;
  lowerInput.toLowerCase();

  if (lowerInput == "help") {
    Serial.println("[PROCESSING]      Built-in Command: HELP");
    Serial.println("[OUTPUT RESULT]   Supported operations:");
    Serial.println("                  1. Normal Operations:");
    Serial.println("                     - Arithmetic: 1000 * 90108 + 100194847 - 10999 / 12");
    Serial.println("                     - Functions: sqrt(16), abs(-25), sin(0), cos(0), log(10)");
    Serial.println("                  2. Matrix Operations:");
    Serial.println("                     - Addition: [[1, 2], [3, 4]] + [[5, 6], [7, 8]]");
    Serial.println("                     - Subtraction: [[5, 6], [7, 8]] - [[1, 2], [3, 4]]");
    Serial.println("                     - Matrix Multiplication: [[1, 2], [3, 4]] * [[2, 0], [1, 2]]");
    Serial.println("                     - Scalar Multiplication: 3 * [[1, 2], [3, 4]]");
    Serial.println("                  3. System Commands: info, ping, help");
  } else if (lowerInput == "info" || lowerInput == "status") {
    Serial.println("[PROCESSING]      Built-in Command: SYSTEM INFO");
    Serial.printf("[OUTPUT RESULT]   Chip: %s (Rev %d) | Freq: %d MHz | Free Heap: %d bytes | Uptime: %lu ms\n",
                  ESP.getChipModel(), ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getFreeHeap(), millis());
  } else if (lowerInput == "ping") {
    Serial.println("[PROCESSING]      Built-in Command: PING");
    Serial.println("[OUTPUT RESULT]   PONG from ESP32!");
  } else if (input.indexOf('[') != -1 && input.indexOf(']') != -1) {
    // Matrix Operation Engine
    Serial.println("[PROCESSING]      Parsing Matrix Expression on ESP32 CPU...");

    int opIndex = -1;
    char opChar = 0;
    int bracketLevel = 0;
    
    for (int i = 0; i < (int)input.length(); i++) {
      char c = input.charAt(i);
      if (c == '[') bracketLevel++;
      else if (c == ']') bracketLevel--;
      else if (bracketLevel == 0 && (c == '+' || c == '-' || c == '*')) {
        opIndex = i;
        opChar = c;
        break;
      }
    }

    if (opIndex == -1) {
      Serial.println("[OUTPUT ERROR]    Matrix expression requires an operator (+, -, *) between operands");
    } else {
      String leftStr = trimStr(input.substring(0, opIndex));
      String rightStr = trimStr(input.substring(opIndex + 1));

      Matrix A, B, C;
      String errStr = "";

      bool isLeftMatrix = leftStr.startsWith("[") && leftStr.endsWith("]");
      bool isRightMatrix = rightStr.startsWith("[") && rightStr.endsWith("]");

      if (isLeftMatrix && isRightMatrix) {
        if (!parseMatrix(leftStr, A, errStr) || !parseMatrix(rightStr, B, errStr)) {
          Serial.printf("[OUTPUT ERROR]    Matrix Parsing Error: %s\n", errStr.c_str());
        } else {
          bool ok = false;
          if (opChar == '+') ok = addMatrices(A, B, C, errStr);
          else if (opChar == '-') ok = subtractMatrices(A, B, C, errStr);
          else if (opChar == '*') ok = multiplyMatrices(A, B, C, errStr);

          if (ok) {
            const char* opName = (opChar == '+') ? "Matrix Addition" : (opChar == '-') ? "Matrix Subtraction" : "Matrix Multiplication";
            printMatrixResult(C, opName);
          } else {
            Serial.printf("[OUTPUT ERROR]    %s\n", errStr.c_str());
          }
        }
      } else if (isLeftMatrix && !isRightMatrix) {
        char* endPtr;
        double k = strtod(rightStr.c_str(), &endPtr);
        if (endPtr == rightStr.c_str()) {
          Serial.printf("[OUTPUT ERROR]    Invalid scalar operand: '%s'\n", rightStr.c_str());
        } else if (!parseMatrix(leftStr, A, errStr)) {
          Serial.printf("[OUTPUT ERROR]    Matrix Parsing Error: %s\n", errStr.c_str());
        } else {
          scaleMatrix(A, k, C);
          printMatrixResult(C, "Matrix Scalar Multiplication");
        }
      } else if (!isLeftMatrix && isRightMatrix) {
        char* endPtr;
        double k = strtod(leftStr.c_str(), &endPtr);
        if (endPtr == leftStr.c_str()) {
          Serial.printf("[OUTPUT ERROR]    Invalid scalar operand: '%s'\n", leftStr.c_str());
        } else if (!parseMatrix(rightStr, B, errStr)) {
          Serial.printf("[OUTPUT ERROR]    Matrix Parsing Error: %s\n", errStr.c_str());
        } else {
          scaleMatrix(B, k, C);
          printMatrixResult(C, "Matrix Scalar Multiplication");
        }
      }
    }
  } else {
    // Normal Operations (Scalar Math & Functions)
    Serial.println("[PROCESSING]      Parsing Normal Math Expression on ESP32 CPU...");
    MathParser parser;
    bool ok = false;
    String errStr = "";
    double result = parser.evaluate(input.c_str(), ok, errStr);
    
    if (ok) {
      if (floor(result) == result && fabs(result) < 1e14) {
        Serial.printf("[OUTPUT RESULT]   %s = %.0f\n", input.c_str(), result);
      } else {
        Serial.printf("[OUTPUT RESULT]   %s = %.4f\n", input.c_str(), result);
      }
    } else {
      Serial.printf("[OUTPUT ERROR]    %s (Input: \"%s\")\n", errStr.c_str(), input.c_str());
    }
  }

  unsigned long durationUs = micros() - startTime;
  Serial.printf("[ESP32 LOG]       Exec Time: %lu us | Heap: %d bytes | Status: OK\n",
                durationUs, ESP.getFreeHeap());
  Serial.println("--------------------------------------------------");
  
  digitalWrite(LED_PIN, LOW);
}
