// MATRIX.h: interface for the MATRIX class.
//
//////////////////////////////////////////////////////////////////////

#include "MATRIX.h"
#include <Accelerate/Accelerate.h>

//////////////////////////////////////////////////////////////////////
// Constructor for the full matrix
//////////////////////////////////////////////////////////////////////
MATRIX::MATRIX() :
  _rows(0), _cols(0)
{
  _matrix = NULL;
}

MATRIX::MATRIX(int rows, int cols) :
  _rows(rows), _cols(cols)
{
  _matrix = new float[_rows * _cols];
  clear();
}

MATRIX::MATRIX(int rows, int cols, float* data) :
  _rows(rows), _cols(cols)
{
  _matrix = new float[_rows * _cols];
	memcpy(_matrix, data, _rows * _cols * sizeof(float));
}

MATRIX::MATRIX(const char* filename)
{
  read(filename);
}

MATRIX::MATRIX(const MATRIX& m)
{
  _cols = m._cols;
  _rows = m._rows;

  _matrix = new float[_rows * _cols];
	memcpy(_matrix, m._matrix, _rows * _cols * sizeof(float));
}

MATRIX::~MATRIX()
{
  delete[] _matrix;
}

//////////////////////////////////////////////////////////////////////
// wipe the whole matrix
//////////////////////////////////////////////////////////////////////
void MATRIX::clear()
{
  memset(_matrix, 0, _rows * _cols * sizeof(float));
}

//////////////////////////////////////////////////////////////////////
// resize and wipe the matrix
//////////////////////////////////////////////////////////////////////
void MATRIX::resizeAndWipe(int rows, int cols)
{
  if (_rows != rows || _cols != cols)
  {
    delete[] _matrix;
    _rows = rows;
    _cols = cols;
    _matrix = new float[_rows * _cols];
  }
  clear();
}

//////////////////////////////////////////////////////////////////////
// write the matrix to a file
//////////////////////////////////////////////////////////////////////
void MATRIX::write(const char* filename)
{
  FILE* file;
  file = fopen(filename, "wb");

  // write dimensions
  fwrite((void*)&_rows, sizeof(int), 1, file);
  fwrite((void*)&_cols, sizeof(int), 1, file);

  // always write out as a double
  if (sizeof(float) != sizeof(double))
  {
    double* matrixDouble = new double[_rows * _cols];
    for (int x = 0; x < _rows * _cols; x++)
      matrixDouble[x] = _matrix[x];

    fwrite((void*)matrixDouble, sizeof(double), _rows * _cols, file);
    delete[] matrixDouble;
    fclose(file);
  }
  else
    fwrite((void*)_matrix, sizeof(float), _rows * _cols, file);
  fclose(file);
}

//////////////////////////////////////////////////////////////////////
// read matrix from a file
//////////////////////////////////////////////////////////////////////
bool MATRIX::read(const char* filename)
{
  FILE* file;
  file = fopen(filename, "rb");
  if (file == NULL)
  {
    cout << __FILE__ << " " << __LINE__ << " : File " << filename << " not found! " << endl;
    return false;
  }

  // read dimensions
  fread((void*)&_rows, sizeof(int), 1, file);
  fread((void*)&_cols, sizeof(int), 1, file);

  // read data
  if (_matrix) delete[] _matrix;
  _matrix = new float[_rows * _cols];

  // always read in a double
  if (sizeof(float) != sizeof(double))
  {
    double* matrixDouble = new double[_rows * _cols];
    fread((void*)matrixDouble, sizeof(double), _rows * _cols, file);
    for (int x = 0; x < _rows * _cols; x++)
      _matrix[x] = matrixDouble[x];
    delete[] matrixDouble;
  }
  else 
    fread((void*)_matrix, sizeof(float), _rows * _cols, file);
  fclose(file);

  return true;
}

//////////////////////////////////////////////////////////////////////
// read matrix from a file
//////////////////////////////////////////////////////////////////////
void MATRIX::read(FILE* file)
{
  int oldRows = _rows;
  int oldCols = _cols;

  // read dimensions
  fread((void*)&_rows, sizeof(int), 1, file);
  fread((void*)&_cols, sizeof(int), 1, file);

  // read data -- if the size is the same, try to preserve
  // the pointer address
  if (oldRows != _rows || oldCols != _cols)
  {
    delete[] _matrix;
    _matrix = new float[_rows * _cols];
  }

  // always read in a double
  if (sizeof(float) != sizeof(double))
  {
    double* matrixDouble = new double[_rows * _cols];
    fread((void*)matrixDouble, sizeof(double), _rows * _cols, file);
    for (int x = 0; x < _rows * _cols; x++)
      _matrix[x] = matrixDouble[x];
    delete[] matrixDouble;
  }
  else 
    fread((void*)_matrix, sizeof(float), _rows * _cols, file);
}

//////////////////////////////////////////////////////////////////////
// write matrix to a file
//////////////////////////////////////////////////////////////////////
void MATRIX::write(FILE* file)
{
  // write dimensions
  fwrite((void*)&_rows, sizeof(int), 1, file);
  fwrite((void*)&_cols, sizeof(int), 1, file);

  // always write in a double
  if (sizeof(float) != sizeof(double))
  {
    double* matrixDouble = new double[_rows * _cols];
    for (int x = 0; x < _rows * _cols; x++)
      matrixDouble[x] = _matrix[x];
    fwrite((void*)matrixDouble, sizeof(double), _rows * _cols, file);
    delete[] matrixDouble;
  }
  else 
    fwrite((void*)_matrix, sizeof(float), _rows * _cols, file);
}

//////////////////////////////////////////////////////////////////////
// return transpose of current matrix
//////////////////////////////////////////////////////////////////////
MATRIX MATRIX::transpose() const
{
  MATRIX toReturn(_cols, _rows);

  for (int y = 0; y < _cols; y++)
    for (int x = 0; x < _rows; x++)
      toReturn(y,x) = (*this)(x,y);

  return toReturn;
}

//////////////////////////////////////////////////////////////////////
// Put the transpose of this matrix in output
//////////////////////////////////////////////////////////////////////
void MATRIX::transpose( MATRIX &output )
{
  for (int y = 0; y < _cols; y++)
    for (int x = 0; x < _rows; x++)
      output(y,x) = (*this)(x,y);
}

//////////////////////////////////////////////////////////////////////
// Matrix-vector multiply
//////////////////////////////////////////////////////////////////////
VECTOR operator*(const MATRIX& A, const VECTOR& x) 
{
  VECTOR y(A.rows());

  for (int i = 0; i < A.rows(); i++)
    for (int j = 0; j < A.cols(); j++)
      y(i) += x(j) * A(i, j);

  return y;
}

//////////////////////////////////////////////////////////////////////
// Vector-matrix multiply
//////////////////////////////////////////////////////////////////////
VECTOR operator*(VECTOR& x, MATRIX& A)
{
  assert(A.rows() == x.size());

  VECTOR y(A.cols());
  for (int i = 0; i < A.cols(); i++)
    for (int j = 0; j < A.rows(); j++)
      y[i] += A(j, i) * x(j);
  return y;
}

//////////////////////////////////////////////////////////////////////
// Scale matrix
//////////////////////////////////////////////////////////////////////
MATRIX operator*(float alpha, const MATRIX& A) 
{
  MATRIX y(A.rows(), A.cols());

  for (int i = 0; i < A.rows(); i++)
    for (int j = 0; j < A.cols(); j++)
      y(i,j) = A(i, j) * alpha;

  return y;
}

//////////////////////////////////////////////////////////////////////
// Deep copy equality operator
//////////////////////////////////////////////////////////////////////
MATRIX& MATRIX::operator=(const MATRIX m)
{
  if (m._cols != _cols || m._rows != _rows)
  {
    delete[] _matrix;
    _cols = m._cols;
    _rows = m._rows;

    _matrix = new float[_rows * _cols];
  }
	memcpy(_matrix, m._matrix, _rows * _cols * sizeof(float));
  return *this;
}

//////////////////////////////////////////////////////////////////////
// stomp the contents and convert this to an identity matrix
//////////////////////////////////////////////////////////////////////
void MATRIX::setToIdentity()
{
  this->clear();
  int least = (_rows > _cols) ? _cols : _rows;
  for (int x = 0; x < least; x++)
    (*this)(x,x) = 1.0;
}

//////////////////////////////////////////////////////////////////////
// squared sum of the whole matrix
//////////////////////////////////////////////////////////////////////
float MATRIX::sum2()
{
  float sum = 0.0;
  for (int y = 0; y < _cols; y++)
    for (int x = 0; x < _rows; x++)
      sum += (*this)(x,y) * (*this)(x,y);
  return sum;
}

//////////////////////////////////////////////////////////////////////
// 1 norm of the matrix
// - maximum absolute column sum
//////////////////////////////////////////////////////////////////////
float MATRIX::norm1()
{
  float maxSum = 0.0;
  for (int x = 0; x < _cols; x++)
  {
    float colSum = 0.0;
    for (int y = 0; y < _rows; y++)
      colSum += fabs((*this)(y,x));
    if (colSum > maxSum)
      maxSum = colSum;
  }
  return maxSum;
}

//////////////////////////////////////////////////////////////////////
// inf norm of the matrix
// - maximum absolute row sum
//////////////////////////////////////////////////////////////////////
float MATRIX::normInf()
{
  float maxSum = 0.0;
  for (int x = 0; x < _rows; x++)
  {
    float rowSum = 0.0;
    for (int y = 0; y < _cols; y++)
      rowSum += fabs((*this)(x,y));
    if (rowSum > maxSum)
      maxSum = rowSum;
  }
  return maxSum;
}

//////////////////////////////////////////////////////////////////////
// sum of two matrices
//////////////////////////////////////////////////////////////////////
MATRIX operator+(const MATRIX& A, const MATRIX& B)
{
  MATRIX result(A.rows(), A.cols());
  for (int y = 0; y < A.cols(); y++)
    for (int x = 0; x < A.rows(); x++)
      result(x,y) = A(x,y) + B(x,y);
  return result;
}

//////////////////////////////////////////////////////////////////////
// difference of two matrices
//////////////////////////////////////////////////////////////////////
MATRIX operator-(const MATRIX& A, const MATRIX& B)
{
  MATRIX result(A.rows(), A.cols());
  for (int y = 0; y < A.cols(); y++)
    for (int x = 0; x < A.rows(); x++)
      result(x,y) = A(x,y) - B(x,y);
  return result;
}

//////////////////////////////////////////////////////////////////////
// Get the absolute max entry in the matrix
//////////////////////////////////////////////////////////////////////
float MATRIX::maxAbsEntry()
{
  float maxFound = 0.0;

  for (int x = 0; x < _rows * _cols; x++)
    if (fabs(_matrix[x]) > maxFound)
      maxFound = fabs(_matrix[x]);

  return maxFound;
}

//////////////////////////////////////////////////////////////////////
// write the vector to a Matlab file
//////////////////////////////////////////////////////////////////////
void MATRIX::writeMatlab(string filename, string varname)
{
  FILE* file;
  file = fopen(filename.c_str(), "w");
  fprintf(file, "%s = [", varname.c_str());
  for (int x = 0; x < _rows; x++)
  {
    for (int y = 0; y < _cols; y++)
      fprintf(file, "%f ", (*this)(x,y));
    fprintf(file, "; ");
  }
  fprintf(file, "];\n");

  fclose(file);
}

//////////////////////////////////////////////////////////////////////
// Print matrix to stream
//////////////////////////////////////////////////////////////////////
ostream& operator<<(ostream &out, const MATRIX& matrix)
{
  out << "[" << endl;
  for (int row = 0; row < matrix.rows(); row++)
  {
    for (int col = 0; col < matrix.cols(); col++)
      out << matrix(row, col) << " ";
    out << endl;
  }
  out << "]" << endl;
  return out;
}

//////////////////////////////////////////////////////////////////////
// clamp the entries of the matrix smaller than a threshold to zero
//////////////////////////////////////////////////////////////////////
void MATRIX::clampToZero(const float threshold)
{
  for (int x = 0; x < _rows; x++)
    for (int y = 0; y < _cols; y++)
    {
      if (fabs((*this)(x,y)) < threshold)
        (*this)(x,y) = 0.0;
    }
}

//////////////////////////////////////////////////////////////////////
// sorting routine for the eigensystem
//////////////////////////////////////////////////////////////////////
struct EIGENPAIR {
  float value;
  VECTOR eigenvector;
};

bool operator<(const EIGENPAIR& a, const EIGENPAIR& b) {
  return a.value < b.value;
}

//////////////////////////////////////////////////////////////////////
// solve for the eigensystem of the matrix
//////////////////////////////////////////////////////////////////////
void MATRIX::eigensystem(VECTOR& eigenvalues, MATRIX& eigenvectors)
{
  // basic error checking
  if (_rows != _cols)
  {
    cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " : " << endl;
    cout << " Matrix must be square to get eigenvalues! " << endl;
    return;
  }

  // resize result space
  eigenvalues.resizeAndWipe(_rows);
  eigenvectors.resizeAndWipe(_rows, _rows);

  // OSX specific - clapack stuff
  __CLPK_integer rowsize = _rows;
  __CLPK_integer worksize = 5 * _rows;
  double* work = new double[worksize];
  double* valuesReal = new double[2 * _rows];
  double* valuesImag = valuesReal + _rows;
  double* vectors = new double[_rows * _rows];
  double* matrix = new double[_rows * _cols];

  for (int x = 0; x < _rows * _cols; x++)
    matrix[x] = _matrix[x];

  // the actual LAPACK call
  __CLPK_integer error;
  char V = 'V';
  char N = 'N';
  dgeev_(&V,&N, &rowsize, matrix, &rowsize, 
        valuesReal, valuesImag, 
        vectors, &rowsize, NULL, &rowsize,
        work, &worksize, &error);

  if (error != 0)
  {
    cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " : " << endl;
    cout << " eigenvalue solver bombed!" << endl;
  }

  cout << " Error value: " << error << endl;
  
  // copy the results into sortable form
  vector<EIGENPAIR> pairs;
  for (int x = 0; x < _rows; x++)
  {
    EIGENPAIR pair;
    pair.value = valuesReal[x];

    VECTOR singleVector(_rows);
    for (int y = 0; y < _rows; y++)
      singleVector[y] = vectors[y + x * _cols];
    pair.eigenvector = singleVector;
    pairs.push_back(pair);
  }

  // sort it
  sort(pairs.begin(), pairs.end());

  // copy out results
  eigenvalues.resizeAndWipe(_rows);
  for (int x = 0; x < _rows; x++)
    eigenvalues(x) = pairs[x].value;

  eigenvectors.resizeAndWipe(_rows, _rows);
  for (int x = 0; x < _rows; x++)
    for (int y = 0; y < _rows; y++)
      eigenvectors(x,y) = pairs[y].eigenvector[x];

  // cleanup
  delete[] work;
  delete[] valuesReal;
  delete[] vectors;
  delete[] matrix;
}
