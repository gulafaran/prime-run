/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <GL/freeglut.h>

#include "opengl.h"

static const int FPS = 60;
static GLfloat currentAngleOfRotation = 0.0;

void gl_reshape(GLint w, GLint h) {
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

void gl_display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(currentAngleOfRotation, 0.0, 0.0, 1.0);
    glRectf(-25.0, -25.0, 25.0, 25.0);
    glFlush();
    glutSwapBuffers();
}

void gl_timer(int v) {
    currentAngleOfRotation += 1.0;
    
    if (currentAngleOfRotation > 360.0) {
        currentAngleOfRotation -= 360.0;
    }

    glutPostRedisplay();
    glutTimerFunc(1000/FPS, gl_timer, v);
}


void gl_keyboard(unsigned char k, int x, int y) {
    switch (k) {
        case 27:
            exit(0);
        break;
        default:
        return;
    }
    glutPostRedisplay();
}

void gl_print_vendor() {  
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);

    printf("GPU Vendor: %s \n", vendor);
    printf("GPU Render: %s \n", renderer);
}

void gl_main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 400);
    glutCreateWindow (argv[0]);
    glutReshapeFunc(gl_reshape);
    glutDisplayFunc(gl_display);
    glutTimerFunc(100, gl_timer, 0);
    glutKeyboardFunc(gl_keyboard);
    gl_print_vendor();
    glutMainLoop();
}