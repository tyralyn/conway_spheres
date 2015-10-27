#include <cmath>
#include "COLOR_FIELD_2D.h"
#include "FIELD_2D.h"
#include "VEC3F.h"
#include "MERSENNE_TWISTER.h"
#include <iostream>
#include "QUICKTIME_MOVIE.h"
#include "MATRIX.h"
#include "VECTOR.h"
#include "TimeStamper.h"

#if _WIN32
#include <gl/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#endif

using namespace std;

int xLayer;
int yLayer;
int zLayer;

int testX = 0;
int testY = 0;
int testZ =0;

bool DEBUG = false;
MATRIX* values;//[xDim][yDim][zDim];
MATRIX* values2;//[xDim][yDim][zDim];

int el = 3;
int eu = 3;
int fl = 3;
int fu = 3;

double rotate_y=10; 
double rotate_x=10;
double scale_z=0;

// resolution of the field
int xRes = 40;
int yRes = 40;
int zRes = 40;

int xDim = xRes;
int yDim = yRes;
int zDim = zRes;
int it =0;
float deltaR;
float deltaG;
float deltaB;
int Count=0;

float r = .45;
float ha;
float hb;

bool laplace = false;

bool editMode = false;

// the field being drawn and manipulated
COLOR_FIELD_2D field(xRes, yRes);
COLOR_FIELD_2D potential(xRes, yRes);
COLOR_FIELD_2D cellEditor(xRes*2+1,yRes*2+1);

// the resolution of the OpenGL window -- independent of the field resolution
int xScreenRes = 800;
int yScreenRes = 800;

// Text for the title bar of the window
string windowLabel("Field Viewer");

// mouse tracking variables
int xMouse         = -1;
int yMouse         = -1;
int mouseButton    = -1;
int mouseState     = -1;
int mouseModifiers = -1;

// current grid cell the mouse is pointing at
int xField = -1;
int yField = -1;

// animate the current runEverytime()?
bool animate = false;

// draw the grid over the field?
bool drawingGrid = false;

// print out what the mouse is pointing at?
bool drawingValues = false;

// currently capturing frames for a movie?
bool captureMovie = false;

// the current viewer eye position
VEC3F eyeCenter(0.5, 0.5, 1);

// current zoom level into the field
float zoom = 1.0;

// Quicktime movie to capture to
QUICKTIME_MOVIE movie;

// a random number generator
MERSENNE_TWISTER twister(123456);

const GLfloat light_ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_diffuse[]  = { 5.0f, 5.0f, 5.0f, 1.0f };
const GLfloat light_specular[] = { 5.0f, 5.0f, 5.0f, 1.0f };
const GLfloat light_position[] = { 25.0f, 25.0f, 25.0f, 0.0f }; 
 
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f }; 
 

// forward declare the caching function here so that we can
// put it at the bottom of the file
void runOnce();

// forward declare the timestepping function here so that we can
// put it at the bottom of the file
void runEverytime();

void threeDConway();
void displayFunc();
///////////////////////////////////////////////////////////////////////
// Figure out which field element is being pointed at, set xField and
// yField to them
///////////////////////////////////////////////////////////////////////
void refreshMouseFieldIndex(int x, int y)
{
  // make the lower left the origin
  y = yScreenRes - y;

  float xNorm = (float)x / xScreenRes;
  float yNorm = (float)y / yScreenRes;

  float halfZoom = 0.5 * zoom;
  float xWorldMin = eyeCenter[0] - halfZoom;
  float xWorldMax = eyeCenter[0] + halfZoom;

  // get the bounds of the field in screen coordinates
  //
  // if non-square textures are ever supported, change the 0.0 and 1.0 below
  float xMin = (0.0 - xWorldMin) / (xWorldMax - xWorldMin);
  float xMax = (1.0 - xWorldMin) / (xWorldMax - xWorldMin);

  float yWorldMin = eyeCenter[1] - halfZoom;
  float yWorldMax = eyeCenter[1] + halfZoom;

  float yMin = (0.0 - yWorldMin) / (yWorldMax - yWorldMin);
  float yMax = (1.0 - yWorldMin) / (yWorldMax - yWorldMin);

  float xScale = 1.0;
  float yScale = 1.0;

  if (xRes < yRes)
    xScale = (float)yRes / xRes;
  if (xRes > yRes)
    yScale = (float)xRes / yRes;

  // index into the field after normalizing according to screen
  // coordinates
  xField = xScale * xRes * ((xNorm - xMin) / (xMax - xMin));
  yField = yScale * yRes * ((yNorm - yMin) / (yMax - yMin));

  // clamp to something inside the field
  xField = (xField < 0) ? 0 : xField;
  xField = (xField >= xRes) ? xRes - 1 : xField;
  yField = (yField < 0) ? 0 : yField;
  yField = (yField >= yRes) ? yRes - 1 : yField;
}

///////////////////////////////////////////////////////////////////////
// Print a string to the GL window
///////////////////////////////////////////////////////////////////////
void printGlString(string output)
{
  glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
  //cout<<"printing "<<output<<endl;
  for (unsigned int x = 0; x < output.size(); x++)
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, output[x]);
}

///////////////////////////////////////////////////////////////////////
// dump the field contents to a GL texture for drawing
///////////////////////////////////////////////////////////////////////
void updateTexture(const COLOR_FIELD_2D& texture)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // create a flatten array of the color values
  int totalCells = texture.xRes() * texture.yRes();
  float* flatData = new float[totalCells * 3];

  for (int x = 0; x < totalCells; x++)
  {
    flatData[3 * x] = texture[x][0];
    flatData[3 * x + 1] = texture[x][1];
    flatData[3 * x + 2] = texture[x][2];
  }

  // send the data to the texture
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 
      texture.xRes(), 
      texture.yRes(), 0, 
      GL_RGB, GL_FLOAT, 
      flatData);

  // remember to clean up after ourselves
  delete[] flatData;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEnable(GL_TEXTURE_2D);
}

void editValues() {
  cout<<"enter z layer to edit: ";
  cin>>zLayer;
  cout<<endl;
}

void editCellArrangement() {

}

///////////////////////////////////////////////////////////////////////
// draw a grid over everything
///////////////////////////////////////////////////////////////////////
void drawGrid()
{
  glColor4f(0.1, 0.1, 0.1, 1.0);

  float dx = 1.0 / xRes;
  float dy = 1.0 / yRes;

  if (xRes < yRes)
    dx *= (float)xRes / yRes;
  if (xRes > yRes)
    dy *= (float)yRes / xRes;

  glBegin(GL_LINES);
  for (int x = 0; x < field.xRes() + 1; x++)
  {
    glVertex3f(x * dx, 0, 1);
    glVertex3f(x * dx, 1, 1);
  }
  for (int y = 0; y < field.yRes() + 1; y++)
  {
    glVertex3f(0, y * dy, 1);
    glVertex3f(1, y * dy, 1);
  }
  glEnd();
}

///////////////////////////////////////////////////////////////////////
// GL and GLUT callbacks
///////////////////////////////////////////////////////////////////////
void glutDisplay()
{
  // Make ensuing transforms affect the projection matrix
  glMatrixMode(GL_PROJECTION);

  // set the projection matrix to an orthographic view
  glLoadIdentity();
  float halfZoom = zoom * 0.5;

  glOrtho(-halfZoom, halfZoom, -halfZoom, halfZoom, -10, 10);

  // set the matrix mode back to modelview
  glMatrixMode(GL_MODELVIEW);

  // set the lookat transform
  glLoadIdentity();
  gluLookAt(eyeCenter[0], eyeCenter[1], 1,  // eye
            eyeCenter[0], eyeCenter[1], 0,  // center 
            0, 1, 0);   // up

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float xLength = 1.0;
  float yLength = 1.0;

  if (xRes < yRes)
    xLength = (float)xRes / yRes;
  if (yRes < xRes)
    yLength = (float)yRes / xRes;

  glEnable(GL_TEXTURE_2D); 
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, yLength, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xLength, yLength, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xLength, 0.0, 0.0);
  glEnd();
  glDisable(GL_TEXTURE_2D); 

  // draw the grid, but only if the user wants
  if (drawingGrid)
    drawGrid();

  // if there's a valid field index, print it
  if (xField >= 0 && yField >= 0 &&
      xField < field.xRes() && yField < field.yRes())
  {
    glLoadIdentity();

    // must set color before setting raster position, otherwise it won't take
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    // normalized screen coordinates (-0.5, 0.5), due to the glLoadIdentity
    float halfZoom = 0.5 * zoom;
    glRasterPos3f(-halfZoom* 0.95, -halfZoom* 0.95, 0);

    // build the field value string
    char buffer[256];
    string fieldValue("(");
    sprintf(buffer, "%i", xField);
    fieldValue = fieldValue + string(buffer);
    sprintf(buffer, "%i", yField);
    fieldValue = fieldValue + string(", ") + string(buffer) + string(") = ");

    VEC3F value = field(xField, yField);
    sprintf(buffer, "(%f %f %f)", value[0], value[1], value[2]);
    fieldValue = fieldValue + string(buffer);

    // draw the grid, but only if the user wants
    if (drawingValues)
      printGlString(fieldValue);
  }

  // if we're recording a movie, capture a frame
  if (captureMovie) {
    movie.addFrameGL();
    //cout<<"addingframe "<<endl;
  }

  glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void printCommands()
{
  cout << "=============================================================== " << endl;
  cout << " Color field viewer code for MAT 200C " << endl;
  cout << "=============================================================== " << endl;
  cout << " q           - quit" << endl;
  cout << " v           - type the value of the cell under the mouse" << endl;
  cout << " g           - throw a grid over everything" << endl;
  cout << " a           - start/stop animation" << endl;  
  cout << " m           - start/stop capturing a movie" << endl;
  cout << " r           - read in a PNG file " << endl;
  cout << " w           - write out a PNG file " << endl;
  cout << " left mouse  - pan around" << endl;
  cout << " right mouse - zoom in and out " << endl;
  cout << " shift left mouse - draw on the grid " << endl;
}

///////////////////////////////////////////////////////////////////////
// Map the arrow keys to something here
///////////////////////////////////////////////////////////////////////
void glutSpecial(int key, int x, int y)
{
  if (!editMode) {
    //  Right arrow - increase rotation by 5 degree
    if (key == GLUT_KEY_RIGHT)
      rotate_y += 5;
   
    //  Left arrow - decrease rotation by 5 degree
    else if (key == GLUT_KEY_LEFT)
      rotate_y -= 5;
   
    else if (key == GLUT_KEY_UP)
      rotate_x += 5;
   
    else if (key == GLUT_KEY_DOWN)
      rotate_x -= 5;
   
    //  Request display update
    glutPostRedisplay();
  }
  else {
   //if (key == GLUT_KEY_RIGHT)
     // rotate_y += 5;
   
    //  Left arrow - decrease rotation by 5 degree
    //else if (key == GLUT_KEY_LEFT)
      //rotate_y -= 5;
   
    if (key == GLUT_KEY_UP && zLayer < zDim-2) {
      zLayer++;
      cout<<zLayer<<endl;
   }
    else if (key == GLUT_KEY_DOWN && zLayer >= 1) {
      zLayer--;
      cout<<zLayer<<endl;
   }
    //  Request display update
    glutPostRedisplay();
  }
}


///////////////////////////////////////////////////////////////////////
// Map the keyboard keys to something here
///////////////////////////////////////////////////////////////////////
void glutKeyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 'a':
      animate = !animate;
      break; 
    case 'b':
      laplace= !laplace;
      break;      
    //case 'd':

    case 'g':
      drawingGrid = !drawingGrid;
      break;
    case '?':
      printCommands();
      break;
    case 'v':
      //cout<<"v: "<<drawingValues<<"\n";
      drawingValues = !drawingValues;
      break;
    case 'm':
      // if we were already capturing a movie
      if (captureMovie)
      {
        // write out the movie
        movie.writeMovie("movie.mov");

        // reset the movie object
        movie = QUICKTIME_MOVIE();

        // stop capturing frames
        captureMovie = false;
      }
      else
      {
        cout << " Starting to capture movie. " << endl;
        captureMovie = true;
      }
      break;
    case 'r':
      field.readPNG("input.png");
      xRes = field.xRes();
      yRes = field.yRes();
      break;
    case 'w':
    {
      TimeStamper ts;
      field.writePNG(ts.timestampedFilename("output",".png"));
    }
      break;
    case 'q':
      exit(0);
      break;
    case 'e':
      editMode = !editMode;
      if (editMode) cout<<"Entering edit mode"<<endl<<zLayer<<endl;
      break;
    case'p':
      //gluLookAt(0,0,0,0,-1,0,0,1,0); 
      scale_z+=1.0;
      break;
    case'o':
      //gluLookAt(0,0,0,0,-1,0,0,1,0); 
      scale_z-=1.0;
      break;
    case's':

      break;
    case ' ':
      runEverytime();
      break;
    default:
      break;
  }
}

///////////////////////////////////////////////////////////////////////
// Do something if the mouse is clicked
///////////////////////////////////////////////////////////////////////
void glutMouseClick(int button, int state, int x, int y)
{
  int modifiers = glutGetModifiers();
  mouseButton = button;
  mouseState = state;
  mouseModifiers = modifiers;

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && modifiers & GLUT_ACTIVE_SHIFT)
  {
    int xLoc , yLoc;
    // figure out which cell we're pointing at
    refreshMouseFieldIndex(x,y);
    if (xField%2==1) {
      xLoc = xField/2;
      yLoc = yField/2;
    }
    else {
      xLoc = xField/2;
      yLoc = (yField-1)/2;
    }
    if (values[zLayer](xLoc,yLoc) < 0.5 ) {
      values[zLayer](xLoc,yLoc) = 1.0;
    }
    else {
      values[zLayer](xLoc,yLoc) = 0.0; 
    }
    if (xField%2 ==1 ){
      field(xLoc*2, yLoc*2) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2+1, yLoc*2) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2+1, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
    }
    else {
      field(xLoc*2, yLoc*2) = values[zLayer](xLoc,yLoc)*VEC3F(0.0,0.0,0.0);
      field(xLoc*2, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(0.0,0.0,0.0);
      field(xLoc*2+1, yLoc*2) = values[zLayer](xLoc,yLoc)*VEC3F(0.0,0.0,0.0);
      field(xLoc*2+1, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(0.0,0.0,0.0);
    }
    return;
  }



  xMouse = x;  
  yMouse = y;
}

///////////////////////////////////////////////////////////////////////
// Do something if the mouse is clicked and moving
///////////////////////////////////////////////////////////////////////
void glutMouseMotion(int x, int y)
{
  if (mouseButton == GLUT_LEFT_BUTTON && 
      mouseState == GLUT_DOWN && 
      mouseModifiers & GLUT_ACTIVE_SHIFT)
  {
    // figure out which cell we're pointing at
    refreshMouseFieldIndex(x,y);
    
    int xLoc , yLoc;
    // figure out which cell we're pointing at
    refreshMouseFieldIndex(x,y);
    if (xField%2==1) {
      xLoc = xField/2;
      yLoc = yField/2;
    }
    else {
      xLoc = xField/2;
      yLoc = (yField-1)/2;
    }
    if (values[zLayer](xLoc,yLoc) < 0.5 ) {
      values[zLayer](xLoc,yLoc) = 1.0;
    }
    else {
      values[zLayer](xLoc,yLoc) = 0.0; 
    }
    if (xField%2 ==1 ){
      field(xLoc*2, yLoc*2) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2+1, yLoc*2) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2+1, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
    }
    else {
      field(xLoc*2, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2, yLoc*2+1+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2+1, yLoc*2+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
      field(xLoc*2+1, yLoc*2+1+1) = values[zLayer](xLoc,yLoc)*VEC3F(1.0,1.0,1.0);
    }
    return;
  }
    // make sure nothing else is called

  float xDiff = x - xMouse;
  float yDiff = y - yMouse;
  float speed = 0.001;
  
  if (mouseButton == GLUT_LEFT_BUTTON) 
  {
    eyeCenter[0] -= xDiff * speed;
    eyeCenter[1] += yDiff * speed;
  }
  if (mouseButton == GLUT_RIGHT_BUTTON)
    zoom -= yDiff * speed;

  xMouse = x;
  yMouse = y;
}

///////////////////////////////////////////////////////////////////////
// Do something if the mouse is not clicked and moving
///////////////////////////////////////////////////////////////////////
void glutPassiveMouseMotion(int x, int y)
{
  refreshMouseFieldIndex(x,y);
}

///////////////////////////////////////////////////////////////////////
// animate and display new result
///////////////////////////////////////////////////////////////////////
void glutIdle()
{
  if(animate){
      runEverytime();      
  }
  
  if (editMode) updateTexture(field);
  //displayFunc();
  glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////
// open the GLVU window
//////////////////////////////////////////////////////////////////////////////
int glvuWindow()
{
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE| GLUT_RGBA);
  glutInitWindowSize(xScreenRes, yScreenRes); 
  glutInitWindowPosition(10, 10);
  glutCreateWindow(windowLabel.c_str());

  // set the viewport resolution (w x h)
  glViewport(0, 0, (GLsizei) xScreenRes, (GLsizei) yScreenRes);

  // set the background color to gray
  glClearColor(0.1, 0.1, 0.1, 0);

  // register all the callbacks
  glutDisplayFunc(&displayFunc);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glutIdleFunc(&glutIdle);
  glutKeyboardFunc(&glutKeyboard);
  glutSpecialFunc(&glutSpecial);
  glutMouseFunc(&glutMouseClick);
  glutMotionFunc(&glutMouseMotion);
  glutPassiveMotionFunc(&glutPassiveMouseMotion);


    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); 
 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
 
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); 
 
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position); 
 
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess); 

  // enter the infinite GL loop
  glutMainLoop();

  // Control flow will never reach here
  return EXIT_SUCCESS;
}

void primordialSoup() {
  for (int x = 0; x < xDim; x++) {
    for (int y = 0; y < yDim; y++) {
      for (int z = 0; z < zDim; z++) {
        float r = twister.rand();
        if (r > 0.85) values[z](x,y) = 1.0;
        else values[z](x,y) = 0.0;
      }
    }
  }
}

void primordialSeed() {
  values[zDim/2](xDim/2,yDim/2) = 1.0;
  values[zDim/2](xDim/2,yDim/2+1) = 1.0;
  values[zDim/2](xDim/2+1,yDim/2) = 1.0;
  values[zDim/2](xDim/2-1,yDim/2) = 1.0;
  values[zDim/2](xDim/2,yDim/2+1) = 1.0;
  values[zDim/2](xDim/2-1,yDim/2-1) = 1.0;
  values[zDim/2](xDim/2+1,yDim/2-1) = 1.0;

}

void primordialCubeCentered() {
  for (int x = xDim/4; x < xDim - xDim/4; x++) {
    for (int y = yDim/4; y < yDim - yDim/4; y++) {
      for (int z = zDim/4; z < zDim - zDim/4; z++) {
        float r = twister.rand();
        if (r > 0.65) values[z](x,y) = 1.0;
        else values[z](x,y) = 0.0;
      }
    }
  }
}

void primordialCube() {
  for (int x = 1; x < xDim/6; x++) {
    for (int y = 1; y < xDim/6; y++) {
      for (int z = 1; z < zDim/6; z++) {
        float r = twister.rand();
        if (r > 0.25) values[z](x,y) = 1.0;
        else values[z](x,y) = 0.0;
      }
    }
  }
  for (int x = xDim/6-3; x < 2*xDim/6-5; x++) {
    for (int y = xDim/6-3; y < 2*xDim/6-5; y++) {
      for (int z = xDim/6-3; z < 2*zDim/6-5; z++) {
        float r = twister.rand();
        if (r > 0.25) values[z](x,y) = 1.0;
        else values[z](x,y) = 0.0;
      }
    }
  }
}

void conwaysPrimordial() {
  for (int x = 0; x < xDim; x++) {
    for (int y = 0; y < xDim; y++) {
      float r = twister.rand();
      if (r > 0.5) values[2](x,y) = 1.0;
      else values[2](x,y) = 0.0;
      values[1](x,y)=values[2](x,y);
    }
  }
}


void initMatrix() {
  values = new MATRIX[zDim];
  values2 = new MATRIX[zDim];
  for (int i = 0; i < zDim; i++) {
    values[i]=MATRIX(xDim,yDim);
    values2[i]=MATRIX(xDim,yDim);
  } 
  ha = r*sqrt(4.0-1.0/(sqrt(3.0)/2.0)*(sqrt(3.0)/2.0));
  hb = r*(sqrt(3.0)/3.0);
  primordialCubeCentered();
  //primordialSeed();
  //primordialSoup();
  //conwaysPrimordial();
  /*for (int i = 0; i < xDim; i++) {
    for (int j = 0; j < yDim; j++) {
      for (int k = 0; k < zDim ; k++) {
        if (i*j*k%4 == 0) values[k](i,j) = 1;
        else values[k](i,j) = 0;
      }
    }
  }
    values[zDim/2](xDim/2,yDim/2) =1;
    values[zDim/2-1](xDim/2,yDim/2) =1;

    values[zDim/2](xDim/2,yDim/3) =1;
    values[zDim/2](xDim/2,yDim/3-1) =1;

    values[zDim/2](xDim/3,yDim/2) =1;*/

  /*values[zDim/2](xDim/2-1, yDim/2-1) =1;
  values[zDim/2](xDim/2, yDim/2-1) =1;
  values[zDim/2](xDim/2+1, yDim/2-1) =1;
  values[zDim/2](xDim/2-1, yDim/2+1) =1;
  values[zDim/2](xDim/2, yDim/2+1) =1;
  values[zDim/2](xDim/2+1, yDim/2+1) =1;
  values[zDim/2](xDim/2, yDim/2) =1;

  //cout<<"end init matrix\n";*/
}

void initEditor() {
  zLayer = 1;
  editMode = false;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{

  initEditor();
  initMatrix();
  // In case the field is rectangular, make sure to center the eye
  if (xRes < yRes)
  {
    float xLength = (float)xRes / yRes;
    eyeCenter[0] = xLength * 0.5;
  }
  if (yRes < xRes)
  {
    float yLength = (float)yRes / xRes;
    eyeCenter[1] = yLength * 0.5;
  }

  runOnce();

  // initialize GLUT and GL
  glutInit(&argc, argv);

  // open the GL window
  glvuWindow();
  return 1;
}

void drawFrontSquare( float xOffset, float yOffset,float zOffset, float alpha ) {
  glBegin(GL_POLYGON);
  glColor4f( 0.0, 1.0, 1.0, alpha );     glVertex3f(  0.35+xOffset, -0.35+yOffset, -0.35+zOffset );      // P1 is red
  glColor4f( 1.0, 0.0, 1.0, alpha );     glVertex3f(  0.35+xOffset,  0.35+yOffset, -0.35+zOffset );      // P2 is green
  glColor4f( 0.0, 1.0, 0.0, alpha );     glVertex3f( -0.35+xOffset,  0.35+yOffset, -0.35+zOffset );      // P3 is blue
  glColor4f( 1.0, 1.0, 1.0, alpha );     glVertex3f( -0.35+xOffset, -0.35+yOffset, -0.35+zOffset );      // P4 is purple
  glEnd();
}

void drawBackSquare( float xOffset, float yOffset,float zOffset, float alpha ) {
  glBegin(GL_POLYGON);
  glColor4f( 1.0, 0.0, 0.0, alpha );     glVertex3f(  0.35+xOffset, -0.35+yOffset, 0.35+zOffset );
  glColor4f( 1.0, 1.0, 0.0, alpha );     glVertex3f(  0.35+xOffset,  0.35+yOffset, 0.35+zOffset );
  glColor4f( 1.0, 0.0, 1.0, alpha );     glVertex3f( -0.35+xOffset,  0.35+yOffset, 0.35+zOffset );
  glColor4f( 1.0, 1.0, 1.0, alpha );     glVertex3f( -0.35+xOffset, -0.35+yOffset, 0.35+zOffset );
  glEnd();
}

void drawBottomSquare( float xOffset, float yOffset,float zOffset, float alpha ) {
  glBegin(GL_POLYGON);
  glColor4f( 1.0, 1.0, 1.0, alpha );     glVertex3f(  0.35+xOffset, -0.35+yOffset, -0.35+zOffset );
  glColor4f( 0.0, 0.0, 1.0, alpha );     glVertex3f(  0.35+xOffset, -0.35+yOffset,  0.35+zOffset );
  glColor4f( 1.0, 0.0, 1.0, alpha );     glVertex3f( -0.35+xOffset, -0.35+yOffset,  0.35+zOffset );
  glColor4f( 1.0, 0.0, 0.0, alpha );     glVertex3f( -0.35+xOffset, -0.35+yOffset, -0.35+zOffset );
  glEnd();
}

void drawTopSquare( float xOffset, float yOffset,float zOffset, float alpha ) {
  glBegin(GL_POLYGON);
  //glColor3f(   0.0,  0.0,  1.0 );
  glColor4f( 1.0, 1.0, 0.0, alpha );     glVertex3f(  0.35+xOffset,  0.35+yOffset,  0.35+zOffset );
  glColor4f( 0.0, 1.0, 0.0, alpha );     glVertex3f(  0.35+xOffset,  0.35+yOffset, -0.35+zOffset );
  glColor4f( 1.0, 1.0, 1.0, alpha );     glVertex3f( -0.35+xOffset,  0.35+yOffset, -0.35+zOffset );
  glColor4f( 1.0, 0.0, 1.0, alpha );     glVertex3f( -0.35+xOffset,  0.35+yOffset,  0.35+zOffset );
  glEnd();
}

void drawLeftSquare( float xOffset, float yOffset,float zOffset, float alpha ) {
  glBegin(GL_POLYGON);
  glColor4f( 1.0, 0.0, 1.0, alpha );     glVertex3f( -0.35+xOffset, -0.35+yOffset,  0.35+zOffset );
  glColor4f( 0.0, 1.0, 1.0, alpha );     glVertex3f( -0.35+xOffset,  0.35+yOffset,  0.35+zOffset );
  glColor4f( 0.0, 0.0, 1.0, alpha );     glVertex3f( -0.35+xOffset,  0.35+yOffset, -0.35+zOffset );
  glColor4f( 1.0, 0.0, 1.0, alpha );     glVertex3f( -0.35+xOffset, -0.35+yOffset, -0.35+zOffset );
  glEnd();  
}

void drawRightSquare( float xOffset, float yOffset,float zOffset, float alpha ) {
  glBegin(GL_POLYGON);
  glColor4f( 1.0, 1.0, 0.0, alpha );     glVertex3f( 0.35+xOffset, -0.35+yOffset, -0.35+zOffset );
  glColor4f( 0.0, 1.0, 0.0, alpha );     glVertex3f( 0.35+xOffset,  0.35+yOffset, -0.35+zOffset );
  glColor4f( 0.0, 1.0, 1.0, alpha );     glVertex3f( 0.35+xOffset,  0.35+yOffset,  0.35+zOffset );
  glColor4f( 1.0, 1.0, 1.0, alpha );     glVertex3f( 0.35+xOffset, -0.35+yOffset,  0.35+zOffset );
  glEnd();  
}

void drawCube(float xOffset, float yOffset,float zOffset, float alpha) {
  drawFrontSquare( xOffset, yOffset, zOffset, alpha );
  drawBackSquare( xOffset, yOffset, zOffset, alpha );
  drawLeftSquare( xOffset, yOffset, zOffset, alpha );
  drawRightSquare( xOffset, yOffset, zOffset, alpha );
  drawTopSquare( xOffset, yOffset, zOffset, alpha );
  drawBottomSquare( xOffset, yOffset, zOffset, alpha ); 

}

void drawSphere(float radius, float xOffset, float yOffset,float zOffset, float R, float G, float B) {

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(R,G,B); 
  glPushMatrix();
    glTranslated(xOffset,yOffset,zOffset);
    glutSolidSphere(radius,15,15);
  glPopMatrix(); 
 
  //glutSwapBuffers();
}

bool isAlive(int x, int y, int z) {
  return (values[z](x,y) > 0.05);
}

int numLiveNeighbors(int x, int y, int z) {
  //bool debug = false;
  //f ((x==14 || x==15 || x==16) && (y==14 || y==15 || y==16) && (z==14 || z==15 || z==16)) debug = true;
  int neighbors = 0;
  if (x%2==1) {
    if (isAlive(x-1, y-1, z)) neighbors++;
    if (isAlive(x, y-1, z)) neighbors++;
    if (isAlive(x+1, y-1, z)) neighbors++;
    if (isAlive(x-1, y, z)) neighbors++;
    if (isAlive(x, y, z)) neighbors++;
    if (isAlive(x+1, y, z)) neighbors++;
    if (isAlive(x, y-1, z)) neighbors++;
    if (z%1 == 1) {
      if (isAlive(x, y, z+1)) neighbors++;
      if (isAlive(x, y+1, z+1)) neighbors++;
      if (isAlive(x+1, y, z+1)) neighbors++;
      if (isAlive(x, y, z-1)) neighbors++;
      if (isAlive(x, y+1, z-1)) neighbors++;
      if (isAlive(x+1, y, z-1)) neighbors++;

    }
    else {
      if (isAlive(x, y, z+1)) neighbors++;
      if (isAlive(x, y-1, z+1)) neighbors++;
      if (isAlive(x-1, y-1, z+1)) neighbors++;
      if (isAlive(x, y, z-1)) neighbors++;
      if (isAlive(x, y-1, z-1)) neighbors++;
      if (isAlive(x-1, y-1, z-1)) neighbors++;
    }
  }
  else {
    if (isAlive(x, y-1, z)) neighbors++;
    if (isAlive(x-1, y, z)) neighbors++;
    if (isAlive(x, y, z)) neighbors++;
    if (isAlive(x+1, y, z)) neighbors++;
    if (isAlive(x-1, y+1, z)) neighbors++;
    if (isAlive(x, y+1, z)) neighbors++;
    if (isAlive(x+1, y+1, z)) neighbors++;
    if (z%1 == 1) {
      if (isAlive(x, y, z+1)) neighbors++;
      if (isAlive(x, y+1, z+1)) neighbors++;
      if (isAlive(x+1, y+1, z+1)) neighbors++;
      if (isAlive(x, y, z-1)) neighbors++;
      if (isAlive(x, y+1, z-1)) neighbors++;
      if (isAlive(x+1, y+1, z-1)) neighbors++;
    }
    else {
      if (isAlive(x-1, y, z+1)) neighbors++;
      if (isAlive(x, y, z+1)) neighbors++;
      if (isAlive(x, y-1, z+1)) neighbors++;     
      if (isAlive(x-1, y, z-1)) neighbors++;
      if (isAlive(x, y, z-1)) neighbors++;
      if (isAlive(x, y-1, z-1)) neighbors++;      
    }
  }
  /*for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      for (int k = -1; k < 2; k++) {
        //if (debug) cout<<x+i<<" "<<y+j<<" "<<z+k<<": "<< neighbors<<endl;
        if (isAlive(x+i, y+j, z+k) && !(i==0 && j==0 && k == 0)) neighbors++;

      }
    }      
  }*/
  return neighbors;
}

void liveCell(int x, int y, int z) {
  values2[z](x,y) = 1.00;
  //cout<<"reviving cell: "<<x<<" "<<y<<endl;
  //if (values2[z](x,y) < 1.0) values2[z](x,y) += 0.10;
}

void killCell(int x, int y, int z) {
  values2[z](x,y) = 0.00;
  //if (values2[z](x,y) > 0.1) values2[z](x,y) -= 0.10;
}

void threeDConway() {
  //cout<<"insideConway\n";
  if (zDim == 2) {
    //cout<<"DO IT\n";
    for (int x = 0; x < xRes; x++) {
      for (int y = 0; y < yRes; y++) {
        values[1](x,y) = values[0](x,y);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
// This function is called every frame -- do something interesting
// here.
///////////////////////////////////////////////////////////////////////
void runEverytime(){
  testX++; 
  if (testX%10 != 0) return;
  cout<<"-----"<<testX/10<<"-----"<<endl;
  //bool B = false;
  for (int i= 1; i < xDim-1; i++) {
    for (int j = 1; j < yDim-1; j++) {
      for (int k = 1; k < zDim -1; k++) {
        int n = numLiveNeighbors(i,j,k);
        bool b = isAlive(i,j,k);
        if (b) {
          //cout<<values[k](i,j)<<" "<<i<<" "<<j<<" "<<k<<" is alive and has neigbors "<<n<<endl;
          if (n >= el && n <= eu) {
            liveCell(i,j,k);
            //cout<<"living  "<<i<<" "<<j<<" "<<k<<" with "<<n<<endl;
          }
          else {
            killCell(i,j,k);
            //cout<<"killing "<<i<<" "<<j<<" "<<k<<" with "<<n<<endl;
            //B = true; break;
          }
        }
        else{
          if (n >= fl && n <= fu) {
            liveCell(i,j,k);
            //cout<<"reviving "<<i<<" "<<j<<" "<<k<<" with "<<n<<endl;            //B = true; break;
          }
          else {
            killCell (i,j,k);
            //scout<<"staying dead "<<i<<" "<<j<<" "<<k<<" with "<<n<<endl;
          }
        }
          //if (b) values2[k](i,j)=0.0;
          ///else values2[k](i,j)=1.0;
      }
    }
  }
  for (int i = 0; i < xDim; i ++) {
    for (int j = 0; j < yDim; j++) {
      for (int k = 0; k < zDim; k++) {
        values[k](i,j)=values2[k](i,j);
      }
    }
  }
}

void displayFunc()
{   
  if (editMode) {
    //threeDConway();
    for (int y = 0; y < yDim; y++) {
        for (int x = 0; x < xDim; x++) {
          if (x%2 ==1) {
            cellEditor(x*2,y*2) = values[zLayer](x,y);
            cellEditor(x*2,y*2+1) = values[zLayer](x,y);
            cellEditor(x*2+1,y*2) = values[zLayer](x,y);
            cellEditor(x*2+1,y*2+1) = values[zLayer](x,y);
          }
          else {
            cellEditor(x*2,y*2+1) = values[zLayer](x,y);
            cellEditor(x*2,y*2+1+1) = values[zLayer](x,y);
            cellEditor(x*2+1,y*2+1) = values[zLayer](x,y);
            cellEditor(x*2+1,y*2+1+1) = values[zLayer](x,y);
          }
        }
    }
    field = cellEditor;
    // Make ensuing transforms affect the projection matrix
    glMatrixMode(GL_PROJECTION);

    // set the projection matrix to an orthographic view
    glLoadIdentity();
    float halfZoom = zoom * 0.5;

    glOrtho(-halfZoom, halfZoom, -halfZoom, halfZoom, -10, 10);

    // set the matrix mode back to modelview
    glMatrixMode(GL_MODELVIEW);

    // set the lookat transform
    glLoadIdentity();
    gluLookAt(eyeCenter[0], eyeCenter[1], 1,  // eye
              eyeCenter[0], eyeCenter[1], 0,  // center 
              0, 1, 0);   // up

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float xLength = 1.0;
    float yLength = 1.0;

    if (xRes < yRes)
      xLength = (float)xRes / yRes;
    if (yRes < xRes)
      yLength = (float)yRes / xRes;

    glEnable(GL_TEXTURE_2D); 
    glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(0.0, yLength, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xLength, yLength, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xLength, 0.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D); 

    // draw the grid, but only if the user wants999999999999999999999999999999999999999999999999
    if (drawingGrid)
      drawGrid();


  }
  else {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  // Reset transformations
   glLoadIdentity();
 
  // Other Transformations
  // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
  // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included
 
  // Rotate when user changes rotate_x and rotate_y
    glRotatef( rotate_x, 1.0, 0.0, 0.0 );
    glRotatef( rotate_y, 0.0, 1.0, 0.0 );

 
  // Other Transformations
    glScalef( 1.0/((float)xRes/2+scale_z), 1.0/((float)yRes/2+scale_z), 1.0/((float)zRes/2+scale_z) );          // Not included
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //drawSphere(r, 0, 0 , 0);
    float zOffset = ha;
    float yOffset = r;
    float xOffset = sqrt(3.0)*r;
    float yStagger = 0.0;
    float zStaggerX = hb;
    float zStaggerY = r;
        //cout<<zOffset<<endl;
    for (int z = 0; z < zDim; z++) {
      zStaggerX = (z%2==1) ? hb : 0.0;
      zStaggerY = (z%2==1) ? r : 0.0;
      for (int x = 0; x < xDim; x++) {
        yStagger = (x%2==1) ? r : 0.0;
        for (int y = 0; y < yDim; y++) {
          float xVal = x*xOffset-xDim/2+zStaggerX;
          float yVal = (float)y*2.0*r-yDim/2 + yStagger+zStaggerY;
          float zVal = z*zOffset-zRes/2;
          if (isAlive(x,y,z) ) drawSphere(r, xVal, yVal, zVal, (float)(xVal+10.0)/(float)xDim, (float)(yVal+10.0)/(float)yDim, (float)(zVal+10.0)/(float)zDim);
          //drawCube(i-xDim/2,j-yDim/2,k-zRes/2, values[k](i,j)/2.0);
        }
      }
    }
    //glutSwapBuffers();
 }
    //cout<<"test: "<<xField<<" "<<yField<<endl;
    // if there's a valid field index, print it
  if (xField >= 0 && yField >= 0 &&
        xField < field.xRes() && yField < field.yRes()) {
    glLoadIdentity();

      // must set color before setting raster position, otherwise it won't take
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

      // normalized screen coordinates (-0.5, 0.5), due to the glLoadIdentity
    float halfZoom = 0.5 * zoom;
    glRasterPos3f(-halfZoom* 0.95, -halfZoom* 0.95, 0);

      // build the field value string
    char buffer[256];
    string fieldValue("(");
    sprintf(buffer, "%i", xField);
    fieldValue = fieldValue + string(buffer);
    sprintf(buffer, "%i", yField);
    fieldValue = fieldValue + string(", ") + string(buffer) + string(") = ");

    VEC3F value = field(xField, yField);
    sprintf(buffer, "(%f %f %f)", value[0], value[1], value[2]);
    fieldValue = fieldValue + string(buffer);

    // draw the grid, but only if the user wants
    //cout<<"drawingValues "<<drawingValues<<endl;
    if (drawingValues)
      printGlString(fieldValue);
  }

  // if we're recording a movie, capture a frame
  if (captureMovie)
  movie.addFrameGL();
  glFlush();
  glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////
// This is called once at the beginning so you can precache
// something here
///////////////////////////////////////////////////////////////////////
void runOnce()
{

}

