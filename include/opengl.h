/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */

#pragma once

#include <GL/freeglut.h>

void gl_reshape(GLint w, GLint h);
void gl_display();
void gl_timer(int v);
void gl_keyboard(unsigned char k, int x, int y);
void gl_print_vendor();
void gl_main(int argc, char** argv);