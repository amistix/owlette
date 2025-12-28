#pragma once
#include <vector>
#include <string>
#include <GLES2/gl2.h>

struct FontAtlas
{
    float fontSize;
    
    int textureWidth;
    int textureHeight;

    std::vector<int> glyphX;
    std::vector<int> glyphW;

    std::string charset;

    unsigned char* pixels = nullptr;
    GLuint fontTexture;

    FontAtlas() : fontSize(32.0f), textureWidth(0), textureHeight(0), pixels(nullptr), fontTexture(0) {}
    
    ~FontAtlas() {
        if (pixels) {
            delete[] pixels;
            pixels = nullptr;
        }
        // Note: fontTexture cleanup should be done via cleanupFontRenderer
        // to ensure proper GL context
    }
    
    // Prevent copying to avoid double-free
    FontAtlas(const FontAtlas&) = delete;
    FontAtlas& operator=(const FontAtlas&) = delete;
};