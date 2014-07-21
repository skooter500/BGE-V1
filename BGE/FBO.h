// FBO.h
#ifndef _FBO_H_
#define _FBO_H_

#ifdef _WIN32
 #include <windows.h>
#endif

#include <GL/glu.h>

struct FBO {
    GLuint id, tex, depth;
    GLuint w, h;
};

void   allocateFBO(FBO&, int w, int h);
void deallocateFBO(FBO&);
void   bindFBO(const FBO&);
void unbindFBO();

#endif //_FBO_H_
