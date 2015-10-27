#ifndef VEC4F_H
#define VEC4F_H

//////////////////////////////////////////////////////////////////////
// A three element vector for a point, direction, or color
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>

class VEC4F {
public:

  // constructors
  VEC4F(float scalar = 0);
  VEC4F(float w, float x, float y, float z);
  VEC4F(const VEC4F& v) { *this = v; }
  VEC4F(const float v[3]);

  // accessors
  operator       float*()        { return data; }
  operator const float*() const  { return data; }
  float& operator[](int i)       { return data[i]; }
  float  operator[](int i) const { return data[i]; }
  operator const float*()        { return data; }

  // overloaded operators
  VEC4F& operator=(const VEC4F& v);
  VEC4F& operator=(const float scalar);
  VEC4F& operator+=(const VEC4F& v);
  VEC4F& operator-=(const VEC4F& v);
  VEC4F& operator*=(const float scalar);
  VEC4F& operator/=(const float scalar);

  // support functions
  float magnitude();
  float magnitudeSq();
  void normalize();
  void clear() { (*this) = 0; };
  static VEC4F fabs(const VEC4F& input);

  // the actual data
  union {
     struct { float w,x,y,z; };
     struct { float r,g,b,a; };
     float data[4];
  };
};

VEC4F operator+(const VEC4F& u, const VEC4F& v);
VEC4F operator-(const VEC4F& u, const VEC4F& v);
VEC4F operator-(const VEC4F& v);
float operator*(const VEC4F& u, const VEC4F& v);
VEC4F operator*(const float scalar, const VEC4F& v);
VEC4F operator*(const VEC4F &v, const float scalar);
VEC4F operator/(const VEC4F &v, float scalar);
std::ostream &operator<<(std::ostream &out, const VEC4F& v);

#endif
