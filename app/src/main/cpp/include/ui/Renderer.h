#pragma once
#include <GLES2/gl2.h>

void initRectShader();
void destroyGLResources();
GLuint getRectProgram();
GLint getPosLoc();
GLint getOffsetLoc();
GLint getSizeLoc();
GLint getColorLoc();
GLint getScrollLoc();
