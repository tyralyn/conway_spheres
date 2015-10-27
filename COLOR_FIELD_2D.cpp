#include "COLOR_FIELD_2D.h"
#include <jpeglib.h>
#include <png.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D::COLOR_FIELD_2D(const int& rows, const int& cols) :
  _xRes(rows), _yRes(cols)
{
  _totalCells = _xRes * _yRes;
  _data = new VEC3F[_totalCells];

  for (int x = 0; x < _totalCells; x++)
    _data[x] = 0.0;
}

COLOR_FIELD_2D::COLOR_FIELD_2D(const COLOR_FIELD_2D& m) :
  _xRes(m.xRes()), _yRes(m.yRes())
{
  _totalCells = _xRes * _yRes;
  _data = new VEC3F[_totalCells];

  for (int x = 0; x < _totalCells; x++)
    _data[x] = m[x];
}

COLOR_FIELD_2D::COLOR_FIELD_2D() :
  _xRes(0), _yRes(0), _totalCells(0), _data(NULL)
{
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D::~COLOR_FIELD_2D()
{
  delete[] _data;
}
  
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::clear()
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] = 0.0;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::writePPM(string filename)
{
  FILE *fp;
  unsigned char* pixels = new unsigned char[3 * _totalCells];

  for (int x = 0; x < _totalCells; x++)
  {
    pixels[3 * x] = 255 * _data[x][0];
    pixels[3 * x + 1] = 255 * _data[x][1];
    pixels[3 * x + 2] = 255 * _data[x][2];
  }

  fp = fopen(filename.c_str(), "wb");
  fprintf(fp, "P6\n%d %d\n255\n", _xRes, _yRes);
  fwrite(pixels, 1, _totalCells * 3, fp);
  fclose(fp);
  delete[] pixels;
}

///////////////////////////////////////////////////////////////////////
// code based on example code from
// http://zarb.org/~gc/html/libpng.html  
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::readPNG(string filename)
{
  cout << " Reading in PNG file " << filename.c_str() << endl;

  int width, height;
  png_structp png_ptr;
  png_infop info_ptr;
  png_byte color_type;
  png_byte bit_depth;

  int number_of_passes;
  png_bytep* row_pointers;
  png_byte header[8];    // 8 is the maximum size that can be checked

  // open file and test for it being a png 
  FILE *fp = fopen(filename.c_str(), "rb");
  if (fp == NULL)
  {
    printf("[read_png_file] File %s could not be opened for reading\n", filename.c_str());
    exit(0);
  }
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8))
    printf("[read_png_file] File %s is not recognized as a PNG file\n", filename.c_str());

  // initialize stuff
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
    printf("[read_png_file] png_create_read_struct failed\n");

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    printf("[read_png_file] png_create_info_struct failed\n");

  if (setjmp(png_jmpbuf(png_ptr)))
    printf("[read_png_file] Error during init_io\n");

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);

  // read file
  if (setjmp(png_jmpbuf(png_ptr)))
    printf("[read_png_file] Error during read_image\n");

  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for (int y = 0; y < height; y++)
    row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

  png_read_image(png_ptr, row_pointers);
  fclose(fp);

  // push the data into the member variables
  _xRes = width;
  _yRes = height;
  _totalCells = _xRes * _yRes;

  if (_data) delete[] _data;
  _data = new VEC3F[_totalCells];

  if (color_type == PNG_COLOR_TYPE_GRAY)
  {
    cout << " PNG color type is gray" << endl;
    for (int y = 0; y < _yRes; y++)
      for (int x = 0; x < _xRes; x++)
        (*this)(x,y) = row_pointers[height - 1 - y][x] / 255.0;
  }
  else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    cout << " PNG color type is gray with alpha" << endl;
    for (int y = 0; y < _yRes; y++)
      for (int x = 0; x < _xRes; x++)
        (*this)(x,y) = row_pointers[height - 1 - y][2 * x] / 255.0;
  }
  else if (color_type == PNG_COLOR_TYPE_RGB)
  {
    cout << " PNG color type is RGB" << endl;
    for (int y = 0; y < _yRes; y++)
      for (int x = 0; x < _xRes; x++)
      {
        float r = (float)row_pointers[height - 1 - y][3 * x] / 255.0;
        float g = (float)row_pointers[height - 1 - y][3 * x + 1] / 255.0;
        float b = (float)row_pointers[height - 1 - y][3 * x + 2] / 255.0;
        (*this)(x,y)[0] = r;
        (*this)(x,y)[1] = g;
        (*this)(x,y)[2] = b;
      }
  }
  else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
  {
    cout << " PNG color type is RGB with alpha" << endl;
    for (int y = 0; y < _yRes; y++)
      for (int x = 0; x < _xRes; x++)
      {
        float r = (float)row_pointers[height - 1 - y][4 * x] / 255.0;
        float g = (float)row_pointers[height - 1 - y][4 * x + 1] / 255.0;
        float b = (float)row_pointers[height - 1 - y][4 * x + 2] / 255.0;
        (*this)(x,y)[0] = r;
        (*this)(x,y)[1] = g;
        (*this)(x,y)[2] = b;
      }
  }
  else
  {
    cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " : " << endl;
    cout << " PNG color type is unsupported! " << endl;
    exit(0);
  }

  for (int y = 0; y < height; y++)
    free(row_pointers[y]);
  free(row_pointers);
}

///////////////////////////////////////////////////////////////////////
// code based on example code from
// http://zarb.org/~gc/html/libpng.html  
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::writePNG(string filename)
{
  cout << " Writing out PNG file " << filename.c_str() << endl;

  int width = _xRes; 
  int height = _yRes;

  // copy image data into pointers
  png_bytep* row_pointers;
  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for (int y = 0; y < height; y++)
    row_pointers[y] = (png_byte*) malloc(sizeof(png_byte) * width * 3);

  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
      VEC3F value = (*this)(x,y) * 255;
      for (int i = 0; i < 3; i++)
      {
        value[i] = (value[i] > 255)  ? 255 : value[i];
        value[i] = (value[i] < 0)  ? 0 : value[i];
        row_pointers[height - 1 - y][3 * x + i] = (unsigned char)value[i];
      }
    }

  png_structp png_ptr;
  png_infop info_ptr;
  png_byte color_type = PNG_COLOR_TYPE_RGB;
  png_byte bit_depth = 8;

  // create file
  FILE *fp = fopen(filename.c_str(), "wb");
  if (fp == NULL)
    printf("[write_png_file] File %s could not be opened for writing\n", filename.c_str());

  // initialize stuff
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
    printf("[write_png_file] png_create_write_struct failed\n");

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    printf("[write_png_file] png_create_info_struct failed\n");

  if (setjmp(png_jmpbuf(png_ptr)))
    printf("[write_png_file] Error during init_io\n");

  png_init_io(png_ptr, fp);

  // write header
  if (setjmp(png_jmpbuf(png_ptr)))
    printf("[write_png_file] Error during writing header\n");

  png_set_IHDR(png_ptr, info_ptr, width, height,
       bit_depth, color_type, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  // write bytes
  if (setjmp(png_jmpbuf(png_ptr)))
    printf("[write_png_file] Error during writing bytes\n");

  png_write_image(png_ptr, row_pointers);
  
  // end write
  if (setjmp(png_jmpbuf(png_ptr)))
    printf("[write_png_file] Error during end of write\n");

  png_write_end(png_ptr, NULL);

  // cleanup heap allocation
  for (int y=0; y<height; y++)
    free(row_pointers[y]);
  free(row_pointers);

  fclose(fp);
}

///////////////////////////////////////////////////////////////////////
// jpeglib code based on:
// http://andrewewhite.net/wordpress/2008/09/02/very-simple-jpeg-writer-in-c-c
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::writeJPG(string filename)
{
  cout << " Writing out JPG file " << filename.c_str() << endl;

  FILE* file = fopen(filename.c_str(), "wb");
 
  if (file == NULL)
  {
    cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " : " << endl;
    cout << " Could not open file: " << filename.c_str() << endl;
    exit(0);
  }

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr       jerr;
   
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, file);
   
  cinfo.image_width      = _xRes;
  cinfo.image_height     = _yRes;
  cinfo.input_components = 3;
  cinfo.in_color_space   = JCS_RGB;

  jpeg_set_defaults(&cinfo);
  // set the quality [0..100]
  jpeg_set_quality (&cinfo, 100, true);
  jpeg_start_compress(&cinfo, true);

  // copy data to a char buffer
  unsigned char* buffer = new unsigned char[3 * _totalCells];
  int index = 0;
  for (int y = 0; y < _yRes; y++)
    for (int x = 0; x < _xRes; x++, index++)
    {
      for (int i = 0; i < 3; i++)
      {
        float entry = (*this)(x, _yRes - 1 - y)[i];
        entry = (entry < 0.0) ? 0.0 : entry;
        entry = (entry > 1.0) ? 1.0 : entry;

        buffer[3 * index + i] = (unsigned char) (255 * entry);
      }
    }

  JSAMPROW row_pointer;
 
  while (cinfo.next_scanline < cinfo.image_height) {
    int index = cinfo.next_scanline * 3 * _xRes;
    row_pointer = (JSAMPROW)&buffer[index];
    jpeg_write_scanlines(&cinfo, &row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);

  delete[] buffer;

  fclose(file);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::normalize()
{
  float maxFound = 0.0;
  float minFound = _data[0].magnitude();
  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
    {
      maxFound = (_data[x][y] > maxFound) ? _data[x][y] : maxFound;
      minFound = (_data[x][y] < minFound) ? _data[x][y] : minFound;
    }

  float range = 1.0 / (maxFound - minFound);
  for (int x = 0; x < _totalCells; x++)
  {
    _data[x] -= minFound;
    _data[x] *= range;
  }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::abs()
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] = VEC3F::fabs(_data[x]);

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::resizeAndWipe(int xRes, int yRes)
{
  if (_xRes == xRes && _yRes == yRes)
  {
    clear();
    return;
  }

  if (_data)
    delete[] _data;

  _xRes = xRes;
  _yRes = yRes;
  _totalCells = _xRes * _yRes;

  _data = new VEC3F[_xRes * _yRes];
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] = alpha;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator*=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] *= alpha;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator/=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] /= alpha;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator+=(const float& alpha)
{
  for (int x = 0; x < _totalCells; x++)
    _data[x] += alpha;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator-=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);
  for (int x = 0; x < _totalCells; x++)
    _data[x] -= input[x];

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator+=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);
  for (int x = 0; x < _totalCells; x++)
    _data[x] += input[x];

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator*=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);

  for (int x = 0; x < _totalCells; x++)
  {
    _data[x][0] *= input[x][0];
    _data[x][1] *= input[x][1];
    _data[x][2] *= input[x][2];
  }

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator/=(const COLOR_FIELD_2D& input)
{
  assert(input.xRes() == _xRes);
  assert(input.yRes() == _yRes);

  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
      if (fabs(input[x][y]) > 1e-6)
        _data[x][y] /= input[x][y];
      else
        _data[x][y] = 0;

  return *this;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator*(const COLOR_FIELD_2D& A, const float alpha)
{
  COLOR_FIELD_2D final(A);
  final *= alpha;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator/(const COLOR_FIELD_2D& A, const float alpha)
{
  COLOR_FIELD_2D final(A);
  final /= alpha;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator+(const COLOR_FIELD_2D& A, const COLOR_FIELD_2D& B)
{
  COLOR_FIELD_2D final(A);
  final += B;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator-(const COLOR_FIELD_2D& A, const COLOR_FIELD_2D& B)
{
  COLOR_FIELD_2D final(A);
  final -= B;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator+(const COLOR_FIELD_2D& A, const float alpha)
{
  COLOR_FIELD_2D final(A);
  final += alpha;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator*(const float alpha, const COLOR_FIELD_2D& A)
{
  return A * alpha;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D operator+(const float alpha, const COLOR_FIELD_2D& A)
{
  return A + alpha;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
COLOR_FIELD_2D& COLOR_FIELD_2D::operator=(const COLOR_FIELD_2D& A)
{
  resizeAndWipe(A.xRes(), A.yRes());

  for (int x = 0; x < _totalCells; x++)
    _data[x] = A[x];

  return *this;
}

///////////////////////////////////////////////////////////////////////
// sum of all entries
///////////////////////////////////////////////////////////////////////
VEC3F COLOR_FIELD_2D::sum()
{
  VEC3F total;
  for (int x = 0; x < _totalCells; x++)
    total += _data[x];

  return total;
}

///////////////////////////////////////////////////////////////////////
// take the log
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::log(float base)
{
  float scale = 1.0 / std::log(base);
  for (int x = 0; x < _totalCells; x++)
    for (int y = 0; y < 3; y++)
      _data[x][y] = std::log(_data[x][y]) * scale;
}

///////////////////////////////////////////////////////////////////////
// get the min of the field
///////////////////////////////////////////////////////////////////////
VEC3F COLOR_FIELD_2D::min()
{
  assert(_xRes > 0);
  assert(_yRes > 0);
  VEC3F final = _data[0];

  for (int i = 0; i < _xRes * _yRes; i++)
    for (int j = 0; j < 3; j++)
      final[j] = (_data[i][j] < final[j]) ? _data[i][j] : final[j];

  return final;
}

///////////////////////////////////////////////////////////////////////
// get the max of the field
///////////////////////////////////////////////////////////////////////
VEC3F COLOR_FIELD_2D::max()
{
  assert(_xRes > 0);
  assert(_yRes > 0);
  VEC3F final = _data[0];

  for (int i = 0; i < _xRes * _yRes; i++)
    for (int j = 0; j < 3; j++)
      final[j] = (_data[i][j] > final[j]) ? _data[i][j] : final[j];

  return final;
}

///////////////////////////////////////////////////////////////////////
// set to a checkboard for debugging
///////////////////////////////////////////////////////////////////////
void COLOR_FIELD_2D::setToCheckerboard(int xChecks, int yChecks)
{
  for (int x = 0; x < _xRes; x++)
    for (int y = 0; y < _yRes; y++)
    {
      int xMod = (x / (_xRes / xChecks)) % 2;
      int yMod = (y / (_yRes / yChecks)) % 2;

      if ((xMod && yMod) || (!xMod && !yMod))
        (*this)(x,y) = 1;
    }
}
