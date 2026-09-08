#ifndef MATRIX_ENGINE_H
#define MATRIX_ENGINE_H

#include <Arduino.h>

#define MAX_ROWS 16
#define MAX_COLS 16

struct Matrix {
  int rows = 0;
  int cols = 0;
  double data[MAX_ROWS][MAX_COLS];

  void reset() {
    rows = 0;
    cols = 0;
    for (int r = 0; r < MAX_ROWS; r++) {
      for (int c = 0; c < MAX_COLS; c++) {
        data[r][c] = 0.0;
      }
    }
  }
};

bool parseMatrix(String str, Matrix& mat, String& errStr);
bool addMatrices(const Matrix& A, const Matrix& B, Matrix& C, String& errStr);
bool subtractMatrices(const Matrix& A, const Matrix& B, Matrix& C, String& errStr);
bool multiplyMatrices(const Matrix& A, const Matrix& B, Matrix& C, String& errStr);
void scaleMatrix(const Matrix& A, double scalar, Matrix& C);
void printMatrixResult(const Matrix& mat, const char* opName);

#endif // MATRIX_ENGINE_H
