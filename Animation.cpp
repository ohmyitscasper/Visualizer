// This program demonstrates double buffering for flicker-free animation.
// It spins a white square on a black background.  It comes from Chapter 1
// of the OpenGL Programming Guide, but I made some minor changes, and did
// the animation properly, using timers, not the idle function.  Start the
// animation with the left mouse button and stop it with the right.

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fftw3.h>
#include <math.h>
#include "RtAudio.h"
#include "AudioProcessing.h"

// Set this to true to animate.
static bool spinning = true;

// This is the number of frames per second to render.
static const int FPS = SAMPLE_RATE/FRAMES;
static const int NUM_BOXES = log2(FRAMES)-1; 

// This global variable keeps track of the current orientation of the square.
// It is better to maintain the "state" of the animation globally rather
// than trying to successively accumulate transformations.  Over a period of
// time the approach of accumulating transformation matrices generally
// degrades due to rounding errors.
GLfloat *yScale;

RtAudio adc;


fftw_complex *in;
fftw_complex *out;
fftw_plan p;

// Handles the window reshape event by first ensuring that the viewport fills
// the entire drawing surface.  Then we use a simple orthographic projection
// with a logical coordinate system ranging from -50..50 in the smaller of
// the width or height, and scale the larger dimension to make the whole
// window isotropic.
void reshape(GLint w, GLint h) {
  glViewport(0, 0, w, h);
  GLfloat aspect = (GLfloat)w / (GLfloat)h;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h) {
    // width is smaller, go from -50 .. 50 in width
    glOrtho(-50.0, 50.0, -50.0/aspect, 50.0/aspect, -1.0, 1.0);
  } else {
    // height is smaller, go from -50 .. 50 in height
    glOrtho(-50.0*aspect, 50.0*aspect, -50.0, 50.0, -1.0, 1.0);
  }
}

// Handles the display callback as follows: first clears the window, then draws
// a 50 x 50 rectangle centered at the origin and rotated the correct number
// of degrees around the vector <0,0,1>.  This function ends with a
// 'glutSwapBuffers' call because when the display mode is double buffered,
// drawing takes place on the back buffer; we have to call glutSwapBuffers()
// to show what we have drawn.
void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  for(int i = 0; i < NUM_BOXES; i++) {
    glPushMatrix();
    glScalef(1.0f, yScale[i], 1.0f);
    glRectf(-30.0+5*i, -5.0, -25.0+5*i, 5.0);
    glPopMatrix();
  }

  glFlush();
  glutSwapBuffers();
}

double scalingFn(double x) {
  return (5.0)*x/(500);
}

// Handles the timer by incrementing the angle of rotation and requesting the
// window to display again, provided the program is in the spinning state.
// Since the timer function is only called once, it sets the same function to
// be called again.
void timer(int v) {
  //Gotta add the fourier transform here

  int i;
  double x;
  double avg=0;
  int count, box = 0;
  double max = 0;

  for(i = 0; i < FRAMES; i++) {
      in[i][0] = newBuffer[i];
      in[i][1] = 0;
  }
  p = fftw_plan_dft_1d(FRAMES, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);

  for(i = 0; i < FRAMES; i++) {
        x = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        if(max<x)
          max = x;
  }
  fftw_destroy_plan(p);
  //std::cout << "max: " << max << std::endl;


//  avg = sqrt(out[1][0] * out[1][0] + out[1][1] * out[1][1]) + sqrt(out[0][0] * out[0][0] + out[0][1] * out[0][1]);
//  avg/=2;
//  yScale[0] = avg;
//
//  avg = sqrt(out[2][0] * out[2][0] + out[2][1] * out[2][1]) + sqrt(out[3][0] * out[3][0] + out[3][1] * out[3][1]);
//  avg/=2;
//  yScale[1] = avg;
//
//  avg = sqrt(out[4][0] * out[4][0] + out[4][1] * out[4][1]) + sqrt(out[5][0] * out[5][0] + out[5][1] * out[5][1]) + sqrt(out[6][0] * out[6][0] + out[6][1] * out[6][1]) + sqrt(out[7][0] * out[7][0] + out[7][1] * out[7][1]);
//  avg/=4;
//  yScale[2] = avg;
  for(int a = 1; a < pow(2,NUM_BOXES); a*=2) {
    count = a;

    for(i = a; i < a*2; i++) {
      avg+=sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
    }

    if(a==1) {
      avg+=sqrt(out[0][0] * out[0][0] + out[0][1] * out[0][1]);
      avg/=2;
      yScale[box++] = avg;
      continue;
    }
    avg/=count;
    yScale[box++] = avg;
  }

//  for(unsigned int i = 0; i < FRAMES; i++) {
//    std::cout << "Data: " << (double)Buffer[i] << std::endl;
//  }
  glutPostRedisplay();
  glutTimerFunc(1000/FPS, timer, v);
}


void keyPressed(unsigned char key, int x, int y) {
  if(key=='q') {
    exitFn();   //Finishing the audio processing
    fftw_free(in);
    fftw_free(out);
    exit(0);
  }
  if(key==' ') {
    spinning = !spinning;
  }
}

// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main(int argc, char** argv) {

  GLfloat box[NUM_BOXES];
  yScale = box;

  //Initializing the fourier transform structures
  in = (fftw_complex *)fftw_malloc(FRAMES*sizeof(fftw_complex));
  out = (fftw_complex *)fftw_malloc(FRAMES*sizeof(fftw_complex));

  audioFn();  //Calls the recorder to start recording
  

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(80, 80);
  glutInitWindowSize(800, 500);
  glutCreateWindow("Spinning Square");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutTimerFunc(FPS, timer, 0);
  glutKeyboardFunc(keyPressed);
  glutMainLoop();
}