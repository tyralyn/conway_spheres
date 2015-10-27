// MATRIX.h: interface for the MATRIX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MATRIX_H
#define MATRIX_H

#include <map>
#include <iostream>
#include <cstdio>
#include <vector>
#include "VECTOR.h"
#include "VEC3F.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// An arbitrary dimension matrix class
//////////////////////////////////////////////////////////////////////
class MATRIX {

public:
  MATRIX();
  MATRIX(int rows, int cols);
  MATRIX(int rows, int cols, float* data);
  MATRIX(const char* filename);
  MATRIX(const MATRIX& m);

  virtual ~MATRIX();

  inline float& operator()(int row, int col) {
    return _matrix[row * _cols + col];
  };
 
  // const version
  inline const float operator()(int row, int col) const {
    return _matrix[row * _cols + col];
  };

  int& rows() { return _rows; };
  int& cols() { return _cols; };
  int rows() const { return _rows; };
  int cols() const { return _cols; };

  // wipe the whole matrix
  void clear();

  // write the matrix to a binary file
  // everything is always written as a double
  void write(const char* filename);
  void write(FILE* file);
  void writeMatlab(string filename, string varname);

  // read from a binary file
  // everything is always read in as a double, then
  // converted if necessary
  bool read(const char* filename);
  void read(FILE* file);

  // resize the matrix and wipe to zero
  void resizeAndWipe(int rows, int cols);

  // overload operators
  MATRIX& operator=(const MATRIX m);
  MATRIX& operator-=(const MATRIX& m);
  MATRIX& operator+=(const MATRIX& m);
  MATRIX& operator*=(const float& alpha);

  // return the transpose of the current matrix
  MATRIX transpose() const;

  void transpose( MATRIX &output );

  // raw data pointer
  float* data() { return _matrix; };

  // stomp everything and set to identity
  void setToIdentity();

  // squared sum of matrix entries
  float sum2();

  // norms of matrix
  float norm1();
  float normInf();
  float norm2() const;
 
  // trace of the matrix
  float trace();

  // max absolute entry
  float maxAbsEntry();

  // clamp the entries of the matrix smaller than a threshold to zero
  void clampToZero(const float threshold);

  // solve for the eigenvectors and values of the matrix
  void eigensystem(VECTOR& eigenvalues, MATRIX& eigenvectors);

protected:
  int _rows;
  int _cols;

  float* _matrix;
};

// overloaded operators
VECTOR operator*(const MATRIX& A, const VECTOR& x);
VECTOR operator*(VECTOR& x, MATRIX& A);
MATRIX operator*(const MATRIX& A, const float alpha);
MATRIX operator*(const float alpha, const MATRIX& A);
MATRIX operator*(const MATRIX& A, const MATRIX& B);
ostream& operator<<(ostream &out, const MATRIX& matrix);
MATRIX operator-(const MATRIX& A, const MATRIX& B);
MATRIX operator+(const MATRIX& A, const MATRIX& B);

// multiply by the transpose of A
VECTOR operator^(const MATRIX& A, const VECTOR& x);

// multiply by the transpose of A -- NOT B!!!
MATRIX operator^(const MATRIX& A, const MATRIX& B);
#endif
