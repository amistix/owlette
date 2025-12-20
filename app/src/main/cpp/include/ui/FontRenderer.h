#pragma once
#include <string>
#include <GLES2/gl2.h>

// Initialize font texture and shader (call once after font data is loaded)
void initFontRenderer();

// Clean up font resources
void cleanupFontRenderer();

// Upload font texture to GPU if not already done
void uploadFontTexture();

// Draw text at pixel coordinates (x, y)
// Color components: r, g, b, a (0.0 to 1.0)
// size parameter is currently unused but kept for API compatibility
void drawText(const std::string& text,
              float x, float y,
              float r, float g, float b, float a,
              float size = 32.0f);

// Measure text width in pixels
float measureText(const std::string& text);

// Get font height in pixels
float getFontHeight();