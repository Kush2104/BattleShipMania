#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Texture loading function
GLuint LoadBMP(const char *filename);
void Cylinder(double x, double y, double z, double radius, double height);
void Cone(double x, double y, double z, double radius, double height);
void Cube(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az);
void Sphere (double x, double y, double z, double radius);

#endif
