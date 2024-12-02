#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// Utility functions for shapes and textures
GLuint LoadBMP(const char *filename);
void Cylinder(double x, double y, double z, double radius, double height);
void Cone(double x, double y, double z, double radius, double height);
void Cube(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az);
void Sphere(double x, double y, double z, double radius);

#endif