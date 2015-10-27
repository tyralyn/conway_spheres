#include "VEC4F.h"

///////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////
VEC4F::VEC4F(float scalar) :
  r(scalar), g(scalar), b(scalar), a(scalar)
{
}

VEC4F::VEC4F(float w, float x, float y, float z) :
  r(w), g(x), b(y), a(z) 
{
}

VEC4F::VEC4F(const float v[4]) :
  r(v[0]), g(v[1]), b(v[2]), a(v[3])
{
}

///////////////////////////////////////////////////////////////////////
// Support functions
///////////////////////////////////////////////////////////////////////
float VEC4F::magnitude()
{ 
  return sqrt((*this) * (*this)); 
}

float VEC4F::magnitudeSq() 
{ 
  return (*this) * (*this); 
}

void VEC4F::normalize() 
{
  float l = this->magnitudeSq();
  *this /= (l < 1.0 && l > 0.0) ? sqrt(l) : 1.0;  
}

VEC4F VEC4F::fabs(const VEC4F& input)
{
  VEC4F final(input);
  final[0] = std::fabs(final[0]);
  final[1] = std::fabs(final[1]);
  final[2] = std::fabs(final[2]);
  final[3] = std::fabs(final[3]);

  return final;
}

///////////////////////////////////////////////////////////////////////
// Overloaded operators
///////////////////////////////////////////////////////////////////////
VEC4F& VEC4F::operator=(const VEC4F& v) 
{ 
  data[0] = v[0]; 
  data[1] = v[1]; 
  data[2] = v[2];  
  data[3] = v[3];  
  return *this; 
}

VEC4F& VEC4F::operator=(const float scalar)
{ 
  data[0] = data[1] = data[2] = data[3] = scalar; 
  return *this; 
}

VEC4F& VEC4F::operator+=(const VEC4F& v)
{ 
  data[0] += v[0];
  data[1] += v[1];
  data[2] += v[2];
  data[3] += v[3];
  return *this; 
}

VEC4F& VEC4F::operator-=(const VEC4F& v)
{ 
  data[0] -= v[0];
  data[1] -= v[1];
  data[2] -= v[2];
  data[3] -= v[3];
  return *this;
}

VEC4F& VEC4F::operator*=(float scalar)
{ 
  data[0] *= scalar;
  data[1] *= scalar;
  data[2] *= scalar;
  data[3] *= scalar;
  return *this; 
}

VEC4F& VEC4F::operator/=(float scalar)
{ 
  data[0] /= scalar;
  data[1] /= scalar;
  data[2] /= scalar;
  data[3] /= scalar;
  return *this;
}

VEC4F operator+(const VEC4F& u, const VEC4F& v)
{ 
  return VEC4F(u[0] + v[0], u[1] + v[1], u[2] + v[2], u[3] + v[3]); 
}

VEC4F operator-(const VEC4F& u, const VEC4F& v)
{ 
  return VEC4F(u[0] - v[0], u[1] - v[1], u[2] - v[2], u[3] - v[3]);
} 

VEC4F operator-(const VEC4F& v)
{ 
  return VEC4F(-v[0], -v[1], -v[2], -v[3]);
}

float operator*(const VEC4F& u, const VEC4F& v)
{ 
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2] + u[3] * v[3]; 
}

VEC4F operator*(const float scalar, const VEC4F& v)
{ 
  return VEC4F(v[0] * scalar, v[1] * scalar, v[2] * scalar, v[3] * scalar);
}

VEC4F operator*(const VEC4F& v, const float scalar)
{ 
  return VEC4F(v[0] * scalar, v[1] * scalar, v[2] * scalar, v[3] * scalar);
}

VEC4F operator/(const VEC4F &v, float scalar)
{ 
  return VEC4F(v[0] / scalar, v[1] / scalar, v[2] / scalar, v[3]  / scalar); 
}

std::ostream &operator<<(std::ostream &out, const VEC4F& v)
{ 
  return out << "(" << v[0] << ", " << v[1] << ", " << v[2] << "," << v[3] << ")"; 
}
