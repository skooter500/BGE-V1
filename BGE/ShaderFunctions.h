#pragma once
#include <GL/gl.h>

GLint getUniLoc(const GLuint program, const GLchar *name);
void  printShaderInfoLog(GLuint obj);
void  printProgramInfoLog(GLuint obj);

const GLchar* GetShaderSource(const char* filename);
GLuint loadShaderFile(const char* filename, const unsigned long Type);
GLuint makeShaderByName(const char* name);
