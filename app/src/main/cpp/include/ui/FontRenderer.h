#pragma once
#include <string>
#include <GLES2/gl2.h>  // ✅ Add this include for GLuint and GLint types

struct FontAtlas;

// Initialize the font rendering system (creates shader program and buffers)
void initFontRenderer();

// Clean up font rendering resources
void cleanupFontRenderer(FontAtlas& atlas);

// Upload font texture to GPU (call once after creating atlas)
void uploadFontTexture(FontAtlas& atlas);

// Draw text at pixel coordinates with specified color
void drawText(const std::string& text,
              float x, float y,
              float r, float g, float b, float a,
              FontAtlas& atlas);

// Measure the width of text in pixels
float measureText(const std::string& text, const FontAtlas& atlas);

// Get the height of the font in pixels
float getFontHeight(const FontAtlas& atlas);

// Shader-related functions (for internal use)
GLuint getRectProgram();
GLint getOffsetLoc();
GLint getSizeLoc();
GLint getColorLoc();
GLint getPosLoc();
void initRectShader();
void destroyGLResources();