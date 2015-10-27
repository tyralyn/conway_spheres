// VECTOR.h: interface for the VECTOR class.
//
//////////////////////////////////////////////////////////////////////

#include "VECTOR.h"

bool VECTOR::printVertical = true;

//////////////////////////////////////////////////////////////////////
// Constructor for the full vector
//////////////////////////////////////////////////////////////////////
VECTOR::VECTOR() :
  _size(0)
{
  _vector = NULL;
}

VECTOR::VECTOR(int size) :
  _size(size)
{
  _vector = new float[_size];
  clear();
}

VECTOR::VECTOR(const char* filename)
{
  read(filename);
}

VECTOR::VECTOR(const VECTOR& v) 
{
  _size = v._size;
  _vector = new float[_size];
  for (int x = 0; x < _size; x++)
    _vector[x] = v._vector[x];
}

VECTOR::VECTOR(const vector<float>& v)
{
  _size = (int)v.size();
  _vector = new float[_size];
  for (int x = 0; x < _size; x++)
    _vector[x] = v[x];
}

VECTOR::VECTOR(const vector<int>& v)
{
  _size = (int)v.size();
  _vector = new float[_size];
  for (int x = 0; x < _size; x++)
    _vector[x] = v[x];
}

VECTOR::VECTOR(FILE* file)
{
  // read dimensions
  fread((void*)&_size, sizeof(int), 1, file);

  // read data
  _vector = new float[_size];
  if (sizeof(float) == sizeof(float))
  {
    double* vecDouble = new double[_size];
    fread((void*)vecDouble, sizeof(double), _size, file);
    for (int x = 0; x < _size; x++)
      _vector[x] = vecDouble[x];
    delete[] vecDouble;
  }
  else
    fread((void*)_vector, sizeof(double), _size, file);
}

VECTOR::~VECTOR()
{
  delete[] _vector;
}

//////////////////////////////////////////////////////////////////////
// wipe the whole vector
//////////////////////////////////////////////////////////////////////
void VECTOR::clear()
{
  memset(_vector, 0.0, _size * sizeof(float));
}

//////////////////////////////////////////////////////////////////////
// resize and wipe the vector
//////////////////////////////////////////////////////////////////////
void VECTOR::resizeAndWipe(int size)
{
  if (_size != size)
  {
    if ( _vector ) delete[] _vector;
    _size = size;
    _vector = new float[_size];
  }
  clear();
}

//////////////////////////////////////////////////////////////////////
// resize and copy the vector
//////////////////////////////////////////////////////////////////////
void VECTOR::resizeAndCopy(int size)
{
  // if it's the same size, do nothing
  if (size == _size)
    return;
 
  // cache old data
  int oldSize = _size;
  float* oldData = _vector;
 
  // get the new size
  _size = size;
  int smaller = (oldSize < _size) ? oldSize : _size;

  // allocate new space
  _vector = new float[_size];

  // do the copy
  for (int x = 0; x < smaller; x++)
    _vector[x] = oldData[x];

  // zero out the leftovers (if there are any)
  for (int x = smaller; x < _size; x++)
    _vector[x] = 0.0;
 
  // clean up old data
  delete[] oldData;
}

//////////////////////////////////////////////////////////////////////
// write the vector to a file
//////////////////////////////////////////////////////////////////////
void VECTOR::write(const char* filename)
{
  FILE* file;
  file = fopen(filename, "wb");
  if (file == NULL)
  {
    cout << " Could not open file " << filename << "!" << endl;
    return;
  }

  // write dimensions
  fwrite((void*)&_size, sizeof(int), 1, file);

  // write data
  if (sizeof(float) == sizeof(float))
  {
    double* vecDouble = new double[_size];
    for (int x = 0; x < _size; x++)
      vecDouble[x] = _vector[x];
    fwrite((void*)vecDouble, sizeof(double), _size, file);
    delete[] vecDouble;
  } 
  else
    fwrite((void*)_vector, sizeof(float), _size, file);
  fclose(file);
}

//////////////////////////////////////////////////////////////////////
// write the vector to a Matlab file
//////////////////////////////////////////////////////////////////////
void VECTOR::writeMatlab(string filename, string varname)
{
  FILE* file;
  file = fopen(filename.c_str(), "w");
  fprintf(file, "%s = [", varname.c_str());
  for (int x = 0; x < _size; x++)
    fprintf(file, "%8.16f ", _vector[x]);
  fprintf(file, "];\n");

  fclose(file);
}

//////////////////////////////////////////////////////////////////////
// write the vector to a file
//////////////////////////////////////////////////////////////////////
void VECTOR::write(FILE* file)
{
  // write dimensions
  fwrite((void*)&_size, sizeof(int), 1, file);

  // write data
  if (sizeof(float) == sizeof(float))
  {
    double* vecDouble = new double[_size];
    for (int x = 0; x < _size; x++)
      vecDouble[x] = _vector[x];
    fwrite((void*)vecDouble, sizeof(double), _size, file);
    delete[] vecDouble;
  } 
  else
    fwrite((void*)_vector, sizeof(float), _size, file);
}

//////////////////////////////////////////////////////////////////////
// read the vector to a file
//////////////////////////////////////////////////////////////////////
void VECTOR::read(FILE* file)
{
  if (_vector) delete[] _vector;

  // read dimensions
  fread((void*)&_size, sizeof(int), 1, file);
  _vector = new float[_size];

  // read data
  if (sizeof(float) == sizeof(float))
  {
    double* vecDouble = new double[_size];
    fread((void*)vecDouble, sizeof(double), _size, file);
    for (int x = 0; x < _size; x++)
      vecDouble[x] = _vector[x];
    delete[] vecDouble;
  } 
  else
    fread((void*)_vector, sizeof(float), _size, file);
}

//////////////////////////////////////////////////////////////////////
// read vector from a file
//////////////////////////////////////////////////////////////////////
bool VECTOR::read(const char* filename)
{
  FILE* file;
  file = fopen(filename, "rb");
  if (file == NULL)
  {
    cout << __FILE__ << " " << __LINE__ << " : File " << filename << " not found! " << endl;
    return false;
  }

  // read dimensions
  fread((void*)&_size, sizeof(int), 1, file);

  // read data
  delete[] _vector;
  _vector = new float[_size];

  if (sizeof(float) == sizeof(float))
  {
    double* vecDouble = new double[_size];
    fread((void*)vecDouble, sizeof(double), _size, file);
    for (int x = 0; x < _size; x++)
      _vector[x] = vecDouble[x];
    delete[] vecDouble;
  }
  else
    fread((void*)_vector, sizeof(float), _size, file);
  fclose(file);

  return true;
}

//////////////////////////////////////////////////////////////////////
// Deep copy equality operator
//////////////////////////////////////////////////////////////////////
VECTOR& VECTOR::operator=(VECTOR m)
{
  if (m.size() != _size)
  {
    delete[] _vector;
    _size= m.size();
    _vector= new float[_size];
  }
	memcpy (_vector, m._vector, _size * sizeof(float));
  return *this;
}

//////////////////////////////////////////////////////////////////////
// Deep copy equality operator
//////////////////////////////////////////////////////////////////////
VECTOR& VECTOR::operator=(vector<float> m)
{
  if (m.size() != (unsigned int)_size)
  {
    delete[] _vector;
    _size = (int)m.size();
    _vector = new float[_size];
  }
  for (unsigned int x = 0; x < m.size(); x++)
    _vector[x] = m[x];

  return *this;
}

//////////////////////////////////////////////////////////////////////
// compute the infinity norm
//////////////////////////////////////////////////////////////////////
float VECTOR::normInf()
{
  if (_size == 0) return 0.0;
  float max = std::fabs(_vector[0]);
  for (int x = 1; x < _size; x++)
    if (std::fabs(_vector[x]) > max)
      max = std::fabs(_vector[x]);
  return max;
}

//////////////////////////////////////////////////////////////////////
// add two vectors
//////////////////////////////////////////////////////////////////////
VECTOR operator+(const VECTOR& x, const VECTOR& y) 
{
  VECTOR z(x.size());

  for (int i = 0; i < x.size(); i++)
    z(i) = x[i] + y[i];

  return z;
}

//////////////////////////////////////////////////////////////////////
// subtract two vectors
//////////////////////////////////////////////////////////////////////
VECTOR operator-(const VECTOR& x, const VECTOR& y) 
{
  VECTOR z(x.size());

  for (int i = 0; i < x.size(); i++)
    z(i) = x[i] - y[i];

  return z;
}

//////////////////////////////////////////////////////////////////////
// scale a vector
//////////////////////////////////////////////////////////////////////
VECTOR operator*(const VECTOR& x, const float& scalar) 
{
  VECTOR z(x.size());

  for (int i = 0; i < x.size(); i++)
    z(i) = x[i] * scalar;

  return z;
}

//////////////////////////////////////////////////////////////////////
// scale a vector
//////////////////////////////////////////////////////////////////////
VECTOR operator*(const float& scalar, const VECTOR& x) 
{
  VECTOR z(x.size());

  for (int i = 0; i < x.size(); i++)
    z(i) = x[i] * scalar;

  return z;
}

//////////////////////////////////////////////////////////////////////
// min of the vector
//////////////////////////////////////////////////////////////////////
float VECTOR::minValue()
{
  if (_size == 0) return 0;
  
  float minFound = _vector[0];
  for (int x = 0; x < _size; x++)
    if (_vector[x] < minFound) minFound = _vector[x];

  return minFound;
}

//////////////////////////////////////////////////////////////////////
// max of the vector
//////////////////////////////////////////////////////////////////////
float VECTOR::maxValue()
{
  if (_size == 0) return 0;
  
  float maxFound = _vector[0];
  for (int x = 0; x < _size; x++)
    if (_vector[x] > maxFound) maxFound = _vector[x];

  return maxFound;
}

//////////////////////////////////////////////////////////////////////
// Take the absolute value of all entries
//////////////////////////////////////////////////////////////////////
void VECTOR::fabs()
{
  for (int x = 0; x < _size; x++)
    _vector[x] = std::fabs(_vector[x]);
}

//////////////////////////////////////////////////////////////////////
// get a sort version of the current vector
//////////////////////////////////////////////////////////////////////
VECTOR VECTOR::sorted()
{
  vector<float> toSort;
  for (int x = 0; x < _size; x++)
    toSort.push_back(_vector[x]);

  sort(toSort.begin(), toSort.end());

  VECTOR final(toSort);
  return final;
}
