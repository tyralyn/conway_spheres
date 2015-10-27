#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "MATRIX.h"
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


int xDim = 10;
int yDim = 10;
int zDim = 10;
MATRIX* values;//[xDim][yDim][zDim];

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void specialKeys();
 
// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
double rotate_y=0; 
double rotate_x=0;
 
void drawCube(float xOffset, float yOffset,float zOffset) {
   // White side - BACK
  glBegin(GL_POLYGON);
  //glColor3f(   1.0,  1.0, 1.0 );
  glColor3f( 1.0, 0.0, 0.0 );     glVertex3f(  0.5+xOffset, -0.5+yOffset, 0.5+zOffset );
  glColor3f( 1.0, 1.0, 0.0 );     glVertex3f(  0.5+xOffset,  0.5+yOffset, 0.5+zOffset );
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5+xOffset,  0.5+yOffset, 0.5+zOffset );
  glColor3f( 1.0, 1.0, 1.0 );     glVertex3f( -0.5+xOffset, -0.5+yOffset, 0.5+zOffset );
  glEnd();
 
  // Purple side - RIGHT
  glBegin(GL_POLYGON);
  //glColor3f(  1.0,  0.0,  1.0 );
  glColor3f( 1.0, 1.0, 0.0 );     glVertex3f( 0.5+xOffset, -0.5+yOffset, -0.5+zOffset );
  glColor3f( 0.0, 1.0, 0.0 );     glVertex3f( 0.5+xOffset,  0.5+yOffset, -0.5+zOffset );
  glColor3f( 0.0, 1.0, 1.0 );     glVertex3f( 0.5+xOffset,  0.5+yOffset,  0.5+zOffset );
  glColor3f( 1.0, 1.0, 1.0 );     glVertex3f( 0.5+xOffset, -0.5+yOffset,  0.5+zOffset );
  glEnd();
 
  // Green side - LEFT
  glBegin(GL_POLYGON);
  //glColor3f(   0.0,  1.0,  0.0 );
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5+xOffset, -0.5+yOffset,  0.5+zOffset );
  glColor3f( 0.0, 1.0, 1.0 );     glVertex3f( -0.5+xOffset,  0.5+yOffset,  0.5+zOffset );
  glColor3f( 0.0, 0.0, 1.0 );     glVertex3f( -0.5+xOffset,  0.5+yOffset, -0.5+zOffset );
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5+xOffset, -0.5+yOffset, -0.5+zOffset );
  glEnd();
 
  // Blue side - TOP
  glBegin(GL_POLYGON);
  //glColor3f(   0.0,  0.0,  1.0 );
  glColor3f( 1.0, 1.0, 0.0 );     glVertex3f(  0.5+xOffset,  0.5+yOffset,  0.5+zOffset );
  glColor3f( 0.0, 1.0, 0.0 );     glVertex3f(  0.5+xOffset,  0.5+yOffset, -0.5+zOffset );
  glColor3f( 1.0, 1.0, 1.0 );     glVertex3f( -0.5+xOffset,  0.5+yOffset, -0.5+zOffset );
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5+xOffset,  0.5+yOffset,  0.5+zOffset );
  glEnd();
 
  // Red side - BOTTOM
  glBegin(GL_POLYGON);
  //glColor3f(   1.0,  0.0,  0.0 );
  glColor3f( 1.0, 1.0, 1.0 );     glVertex3f(  0.5+xOffset, -0.5+yOffset, -0.5+zOffset );
  glColor3f( 0.0, 0.0, 1.0 );     glVertex3f(  0.5+xOffset, -0.5+yOffset,  0.5+zOffset );
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5+xOffset, -0.5+yOffset,  0.5+zOffset );
  glColor3f( 1.0, 0.0, 0.0 );     glVertex3f( -0.5+xOffset, -0.5+yOffset, -0.5+zOffset );
  glEnd();

  glBegin(GL_POLYGON);
 
  glColor3f( 0.0, 1.0, 1.0 );     glVertex3f(  0.5+xOffset, -0.5+yOffset, -0.5+zOffset );      // P1 is red
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f(  0.5+xOffset,  0.5+yOffset, -0.5+zOffset );      // P2 is green
  glColor3f( 0.0, 1.0, 0.0 );     glVertex3f( -0.5+xOffset,  0.5+yOffset, -0.5+zOffset );      // P3 is blue
  glColor3f( 1.0, 1.0, 1.0 );     glVertex3f( -0.5+xOffset, -0.5+yOffset, -0.5+zOffset );      // P4 is purple
 
  glEnd();

}

// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display(){
 
  //  Clear screen and Z-buffer
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
   glScalef( 1.0/15.0, 1.0/15.0, 1.0/15.0 );          // Not included
 
  for (int i = -xDim/2-1; i < xDim/2; i ++) {
    for (int j = -yDim/2-1; j < yDim/2; j++) {
      for (int k = -zDim/2-1; k < zDim/2; k++) {
        drawCube(i,j,k);
      }
    }
  }

  //drawCube(0.0,0.0,1.0);
  //drawCube(0.0,4.0,0.0);

  glFlush();
  glutSwapBuffers();
 
}
 
// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys( int key, int x, int y ) {
 
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
 

void initMatrix() {
  values = new MATRIX[zDim];
  for (int i = 0; i < zDim; i++) {
    values[i]=MATRIX(xDim,yDim);
  } 
}

// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
int main(int argc, char* argv[]){

  //  Initialize GLUT and process user parameters
  glutInit(&argc,argv);
 
  //  Request double buffered true color window with Z-buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 
  // Create window
  glutCreateWindow("Awesome Cube");
 
  //  Enable Z-buffer depth test
  glEnable(GL_DEPTH_TEST);
 
  // Callback functions
  glutDisplayFunc(display);
  glutSpecialFunc(specialKeys);
 
  //  Pass control to GLUT for events
  glutMainLoop();
 
  //  Return to OS
  return 0;
 
}
