#pragma once
#include <string>
#include "ui/TextView.h"
#include <GLES2/gl2.h>

struct FontAtlas;

void initFontRenderer();

void cleanupFontRenderer(FontAtlas& atlas);

void uploadFontTexture(FontAtlas& atlas);

void drawText(const std::string& text,
              float x, float y,
              float r, float g, float b, float a,
              FontAtlas& atlas, ui::TextView* textView);

float measureText(const std::string& text, const FontAtlas& atlas);

float getFontHeight(const FontAtlas& atlas);

GLuint getRectProgram();
GLint getOffsetLoc();
GLint getSizeLoc();
GLint getColorLoc();
GLint getPosLoc();
void initRectShader();
void destroyGLResources();