// VECTOR.h: interface for the VECTOR class.
//
//////////////////////////////////////////////////////////////////////

#ifndef VECTOR_H
#define VECTOR_H

#include <map>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>
#include <cassert>

using namespace std;

//////////////////////////////////////////////////////////////////////
// An arbitrary dimension vector class
//////////////////////////////////////////////////////////////////////
class VECTOR {

public:
  VECTOR();
  VECTOR(int size);
  VECTOR(const char* filename);
  VECTOR(const VECTOR& v);
  VECTOR(FILE* file);
  VECTOR(const vector<float>& v);
  VECTOR(const vector<int>& v);
  ~VECTOR();

  inline float& operator()(int index) { 
    assert(index >= 0);
    assert(index < _size);
    return _vector[index];
  };
  inline const float operator() (int index) const { 
    assert(index >= 0);
    assert(index < _size);
    return _vector[index];
  };
  inline float& operator[](int index) { 
    assert(index >= 0);
    assert(index < _size);
    return _vector[index];
  };
  inline const float operator[] (int index) const { 
    assert(index >= 0);
    assert(index < _size);
    return _vector[index];
  };
 
  int size() const { return _size; };

  // wipe the whole vector
  void clear();

  // write the vector to a binary file
  // everything is always written as a double
  void write(const char* filename);
  void write(FILE* file);
  void writeMatlab(string filename, string varname);

  // read the vector to a binary file
  // everything is always read in as a double, then
  // converted if necessary
  bool read(const char* filename);
  void read(FILE* file);

  // resize the vector and wipe to zero
  void resizeAndWipe(int size);

  // resize the vector and copy its current contents
  void resizeAndCopy(int size);

  // overloaded operators
  VECTOR& operator=(VECTOR m);
  VECTOR& operator=(vector<float> m);
  VECTOR& operator+=(const VECTOR& v);
  VECTOR& operator-=(const VECTOR& v);
  VECTOR& operator*=(const float& alpha);

  // 2 norm
  float norm2();
  
  // Infinity norm
  float normInf();

  // dot product
  float operator*(const VECTOR& vector);

  // raw data pointer
  float* data() { return _vector; };
  const float* dataConst() const { return _vector; };

  // max of all the elements
  float maxValue();

  // min of all the elements
  float minValue();

  // take the absolute value of all entires
  void fabs();

  static bool printVertical;

  // get a sorted version of this vector
  VECTOR sorted();

private:
  int _size;
  float* _vector;
};

//////////////////////////////////////////////////////////////////////
// dump vector to iostream
//////////////////////////////////////////////////////////////////////
inline ostream &operator<<(ostream &out, VECTOR vector)
{
  out << "[";
  if (VECTOR::printVertical)
  {
    out << endl;
    for (int x = 0; x < vector.size(); x++)
      out << vector(x) << endl;
  }
  else
    for (int x = 0; x < vector.size(); x++)
      out << vector(x) << " ";
  out << "]";
  if (VECTOR::printVertical)
    out << endl;
  return out;
}

// overloaded operators
VECTOR operator-(const VECTOR& x, const VECTOR& y);
VECTOR operator+(const VECTOR& x, const VECTOR& y);
VECTOR operator*(const VECTOR& x, const float& scalar);
VECTOR operator*(const float& scalar, const VECTOR& x);

#endif
