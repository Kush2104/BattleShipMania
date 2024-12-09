#ifndef SHADER_UTIL_H
#define SHADER_UTIL_H

#include "utils.h"

// Read text from file
char* ReadShaderFile(const char* file);

// Shader compilation and error checking
void PrintShaderLog(int obj, const char* file);
void PrintProgramLog(int obj);

// Shader creation functions
int CreateShader(int prog, const GLenum type, const char* file);
int CreateShaderProg(const char* vert, const char* frag);
int CreateShaderProgCompute(const char* file);
int CreateShaderGeom(const char* VertFile, const char* GeomFile, const char* FragFile);

// Error handling
void ShaderErrorExit(const char* format, ...);

#endif