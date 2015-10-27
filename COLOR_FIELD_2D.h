#ifndef COLOR_FIELD_2D_H
#define COLOR_FIELD_2D_H

#include <cmath>
#include <string>
#include <iostream>
#include "VEC3F.h"

using namespace std;

class COLOR_FIELD_2D {
public:
  COLOR_FIELD_2D();
  COLOR_FIELD_2D(const int& rows, const int& cols);
  COLOR_FIELD_2D(const COLOR_FIELD_2D& m);
  ~COLOR_FIELD_2D();

  // accessors
  inline VEC3F& operator()(int x, int y) { return _data[y * _xRes + x]; };
  const  VEC3F operator()(int x, int y) const { return _data[y * _xRes + x]; };
  inline VEC3F& operator[](int x) { return _data[x]; };
  const  VEC3F operator[](int x) const { return _data[x]; };
  VEC3F* data() { return _data; };
  VEC3F* const data() const { return _data; };
  const int xRes() const { return _xRes; };
  const int yRes() const { return _yRes; };
  const int totalCells() const { return _totalCells; };

  // common field operations
  void clear();
  void normalize();
  COLOR_FIELD_2D& abs();

  VEC3F min();
  VEC3F max();

  // take the log
  void log(float base = 2.0);
 
  // IO functions
  void writePPM(string filename);
  void writeJPG(string filename);
  void writePNG(string filename);
  void readPNG(string filename);

  // change dimensions and clear the colors
  void resizeAndWipe(int xRes, int yRes);

  // overloaded operators
  COLOR_FIELD_2D& operator=(const float& alpha);
  COLOR_FIELD_2D& operator=(const COLOR_FIELD_2D& A);
  COLOR_FIELD_2D& operator*=(const float& alpha);
  COLOR_FIELD_2D& operator/=(const float& alpha);
  COLOR_FIELD_2D& operator+=(const float& alpha);
  COLOR_FIELD_2D& operator-=(const COLOR_FIELD_2D& input);
  COLOR_FIELD_2D& operator+=(const COLOR_FIELD_2D& input);
  COLOR_FIELD_2D& operator*=(const COLOR_FIELD_2D& input);
  COLOR_FIELD_2D& operator/=(const COLOR_FIELD_2D& input);

  // sum of all entries
  VEC3F sum();
  
  // set to a checkboard for debugging
  void setToCheckerboard(int xChecks = 10, int yChecks = 10);
  
private:
  int _xRes;
  int _yRes;
  int _totalCells;
  VEC3F* _data;
};

COLOR_FIELD_2D operator*(const COLOR_FIELD_2D& A, const float alpha);
COLOR_FIELD_2D operator/(const COLOR_FIELD_2D& A, const float alpha);
COLOR_FIELD_2D operator+(const COLOR_FIELD_2D& A, const float alpha);
COLOR_FIELD_2D operator*(const float alpha, const COLOR_FIELD_2D& A);
COLOR_FIELD_2D operator+(const float alpha, const COLOR_FIELD_2D& A);
COLOR_FIELD_2D operator-(const COLOR_FIELD_2D& A, const COLOR_FIELD_2D& B);
COLOR_FIELD_2D operator+(const COLOR_FIELD_2D& A, const COLOR_FIELD_2D& B);

#endif
