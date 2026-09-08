#include "MatrixEngine.h"
#include <math.h>

static String trimStr(String str) {
  str.trim();
  return str;
}

bool parseMatrix(String str, Matrix& mat, String& errStr) {
  mat.reset();
  str = trimStr(str);

  if (!str.startsWith("[") || !str.endsWith("]")) {
    errStr = "Matrix must be enclosed in brackets [ ... ]";
    return false;
  }

  String content = str.substring(1, str.length() - 1);
  content = trimStr(content);

  if (content.indexOf('[') != -1) {
    int rowIdx = 0;
    int currPos = 0;

    while (currPos < (int)content.length()) {
      int openBracket = content.indexOf('[', currPos);
      if (openBracket == -1) break;
      int closeBracket = content.indexOf(']', openBracket);
      if (closeBracket == -1) {
        errStr = "Missing closing bracket ']' in matrix row";
        return false;
      }

      String rowContent = content.substring(openBracket + 1, closeBracket);
      rowContent = trimStr(rowContent);

      int colIdx = 0;
      int numStart = 0;
      for (int i = 0; i <= (int)rowContent.length(); i++) {
        if (i == (int)rowContent.length() || rowContent.charAt(i) == ',' || rowContent.charAt(i) == ' ') {
          if (i > numStart) {
            String token = trimStr(rowContent.substring(numStart, i));
            if (token.length() > 0) {
              if (rowIdx >= MAX_ROWS || colIdx >= MAX_COLS) {
                errStr = "Matrix dimensions exceed maximum limit (16x16)";
                return false;
              }
              char* endPtr;
              mat.data[rowIdx][colIdx] = strtod(token.c_str(), &endPtr);
              if (endPtr == token.c_str()) {
                errStr = "Invalid number token in matrix: '" + token + "'";
                return false;
              }
              colIdx++;
            }
          }
          numStart = i + 1;
        }
      }

      if (rowIdx == 0) {
        mat.cols = colIdx;
      } else if (colIdx != mat.cols) {
        errStr = "Inconsistent row lengths in matrix";
        return false;
      }

      rowIdx++;
      currPos = closeBracket + 1;
    }

    mat.rows = rowIdx;
  } else {
    int rowIdx = 0;
    int startRowPos = 0;

    for (int i = 0; i <= (int)content.length(); i++) {
      if (i == (int)content.length() || content.charAt(i) == ';') {
        String rowContent = trimStr(content.substring(startRowPos, i));
        if (rowContent.length() > 0) {
          int colIdx = 0;
          int numStart = 0;
          for (int j = 0; j <= (int)rowContent.length(); j++) {
            if (j == (int)rowContent.length() || rowContent.charAt(j) == ',' || rowContent.charAt(j) == ' ') {
              if (j > numStart) {
                String token = trimStr(rowContent.substring(numStart, j));
                if (token.length() > 0) {
                  if (rowIdx >= MAX_ROWS || colIdx >= MAX_COLS) {
                    errStr = "Matrix dimensions exceed maximum limit (16x16)";
                    return false;
                  }
                  char* endPtr;
                  mat.data[rowIdx][colIdx] = strtod(token.c_str(), &endPtr);
                  if (endPtr == token.c_str()) {
                    errStr = "Invalid number token in matrix: '" + token + "'";
                    return false;
                  }
                  colIdx++;
                }
              }
              numStart = j + 1;
            }
          }

          if (rowIdx == 0) {
            mat.cols = colIdx;
          } else if (colIdx != mat.cols) {
            errStr = "Inconsistent row lengths in matrix";
            return false;
          }
          rowIdx++;
        }
        startRowPos = i + 1;
      }
    }
    mat.rows = rowIdx;
  }

  if (mat.rows == 0 || mat.cols == 0) {
    errStr = "Empty matrix provided";
    return false;
  }

  return true;
}

bool addMatrices(const Matrix& A, const Matrix& B, Matrix& C, String& errStr) {
  if (A.rows != B.rows || A.cols != B.cols) {
    errStr = "Dimension mismatch: cannot add " + String(A.rows) + "x" + String(A.cols) + 
             " matrix to " + String(B.rows) + "x" + String(B.cols) + " matrix";
    return false;
  }
  C.rows = A.rows;
  C.cols = A.cols;
  for (int r = 0; r < A.rows; r++) {
    for (int c = 0; c < A.cols; c++) {
      C.data[r][c] = A.data[r][c] + B.data[r][c];
    }
  }
  return true;
}

bool subtractMatrices(const Matrix& A, const Matrix& B, Matrix& C, String& errStr) {
  if (A.rows != B.rows || A.cols != B.cols) {
    errStr = "Dimension mismatch: cannot subtract " + String(B.rows) + "x" + String(B.cols) + 
             " matrix from " + String(A.rows) + "x" + String(A.cols) + " matrix";
    return false;
  }
  C.rows = A.rows;
  C.cols = A.cols;
  for (int r = 0; r < A.rows; r++) {
    for (int c = 0; c < A.cols; c++) {
      C.data[r][c] = A.data[r][c] - B.data[r][c];
    }
  }
  return true;
}

bool multiplyMatrices(const Matrix& A, const Matrix& B, Matrix& C, String& errStr) {
  if (A.cols != B.rows) {
    errStr = "Dimension mismatch: cannot multiply " + String(A.rows) + "x" + String(A.cols) + 
             " matrix by " + String(B.rows) + "x" + String(B.cols) + " matrix (cols of A must equal rows of B)";
    return false;
  }
  C.rows = A.rows;
  C.cols = B.cols;
  for (int r = 0; r < A.rows; r++) {
    for (int c = 0; c < B.cols; c++) {
      double sum = 0.0;
      for (int k = 0; k < A.cols; k++) {
        sum += A.data[r][k] * B.data[k][c];
      }
      C.data[r][c] = sum;
    }
  }
  return true;
}

void scaleMatrix(const Matrix& A, double scalar, Matrix& C) {
  C.rows = A.rows;
  C.cols = A.cols;
  for (int r = 0; r < A.rows; r++) {
    for (int c = 0; c < A.cols; c++) {
      C.data[r][c] = A.data[r][c] * scalar;
    }
  }
}

void printMatrixResult(const Matrix& mat, const char* opName) {
  Serial.printf("[OUTPUT RESULT]   %s Result (%dx%d):\n", opName, mat.rows, mat.cols);
  for (int r = 0; r < mat.rows; r++) {
    Serial.print("                  | ");
    for (int c = 0; c < mat.cols; c++) {
      double val = mat.data[r][c];
      if (floor(val) == val && fabs(val) < 1e12) {
        Serial.printf("%6ld ", (long)val);
      } else {
        Serial.printf("%6.2f ", val);
      }
    }
    Serial.println("|");
  }

  Serial.print("                  Inline: [");
  for (int r = 0; r < mat.rows; r++) {
    Serial.print("[");
    for (int c = 0; c < mat.cols; c++) {
      double val = mat.data[r][c];
      if (floor(val) == val && fabs(val) < 1e12) {
        Serial.printf("%ld", (long)val);
      } else {
        Serial.printf("%.2f", val);
      }
      if (c < mat.cols - 1) Serial.print(", ");
    }
    Serial.print("]");
    if (r < mat.rows - 1) Serial.print(", ");
  }
  Serial.println("]");
}
