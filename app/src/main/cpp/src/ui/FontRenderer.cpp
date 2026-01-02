#include "ui/FontRenderer.h"
#include "ui/FontAtlas.h"
#include <GLES2/gl2.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <cstdio>

#define LOG_TAG "FontRenderer"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Enable detailed logging for debugging (set to 0 for production)
#define FONT_DEBUG_LOGGING 0

static GLuint g_fontShader = 0;
static GLuint g_vbo = 0;
static GLuint g_ibo = 0;

// Shader attribute/uniform locations
static GLint g_uColor = -1;
static GLint g_uTex = -1;
static GLint g_aPos = -1;
static GLint g_aTex = -1;

// Vertex shader (NDC space)
static const char* vertexShaderSrc = R"(
attribute vec2 aPos;
attribute vec2 aTex;
varying vec2 vTex;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vTex = aTex;
}
)";

// Fragment shader with alpha blending
static const char* fragmentShaderSrc = R"(
precision mediump float;
uniform sampler2D uTex;
uniform vec4 uColor;
varying vec2 vTex;
void main() {
    float alpha = texture2D(uTex, vTex).a;
    gl_FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
)";

static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char buf[512];
        glGetShaderInfoLog(shader, 512, nullptr, buf);
        LOGE("Shader compile error: %s", buf);
    }
    return shader;
}

static GLuint createShaderProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char buf[512];
        glGetProgramInfoLog(program, 512, nullptr, buf);
        LOGE("Program link error: %s", buf);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void initFontRenderer() {
    g_fontShader = createShaderProgram();
    if (g_fontShader == 0) {
        LOGE("Failed to create shader program");
        return;
    }
    
    // Get attribute and uniform locations
    g_uColor = glGetUniformLocation(g_fontShader, "uColor");
    g_uTex = glGetUniformLocation(g_fontShader, "uTex");
    g_aPos = glGetAttribLocation(g_fontShader, "aPos");
    g_aTex = glGetAttribLocation(g_fontShader, "aTex");
    
    LOGD("Shader locations - uColor:%d uTex:%d aPos:%d aTex:%d", 
         g_uColor, g_uTex, g_aPos, g_aTex);
    
    // Create VBO and IBO for efficient rendering
    glGenBuffers(1, &g_vbo);
    glGenBuffers(1, &g_ibo);
    
    LOGD("Font renderer initialized - VBO:%d IBO:%d", g_vbo, g_ibo);
}

void cleanupFontRenderer(FontAtlas& atlas) {
    if (atlas.fontTexture != 0) {
        glDeleteTextures(1, &atlas.fontTexture);
        atlas.fontTexture = 0;
    }
    if (g_vbo != 0) {
        glDeleteBuffers(1, &g_vbo);
        g_vbo = 0;
    }
    if (g_ibo != 0) {
        glDeleteBuffers(1, &g_ibo);
        g_ibo = 0;
    }
    if (g_fontShader != 0) {
        glDeleteProgram(g_fontShader);
        g_fontShader = 0;
    }
    LOGD("Font renderer cleaned up");
}

void uploadFontTexture(FontAtlas& atlas) {
    if (atlas.fontTexture != 0) {
        GLboolean isTexture = glIsTexture(atlas.fontTexture);
        if (isTexture) {
            LOGD("Font texture already uploaded and valid (ID: %d)", atlas.fontTexture);
            return;
        } else {
            LOGD("Font texture ID exists but invalid, re-creating");
            atlas.fontTexture = 0;
        }
    }
    
    if (!atlas.pixels) {
        LOGE("❌ Font pixels not loaded! pixels is NULL");
        return;
    }
    
    if (atlas.textureWidth == 0 || atlas.textureHeight == 0) {
        LOGE("❌ Invalid font dimensions: %dx%d", atlas.textureWidth, atlas.textureHeight);
        return;
    }
    
    if (atlas.glyphX.empty() || atlas.glyphW.empty()) {
        LOGE("❌ Glyph data not loaded! glyphX size:%zu glyphW size:%zu", 
             atlas.glyphX.size(), atlas.glyphW.size());
        return;
    }
    
    if (atlas.charset.empty()) {
        LOGE("❌ Charset is empty!");
        return;
    }

    LOGD("✓ Creating font texture...");
    LOGD("  - Dimensions: %dx%d", atlas.textureWidth, atlas.textureHeight);
    LOGD("  - Glyphs: %zu", atlas.glyphX.size());
    LOGD("  - Charset length: %zu", atlas.charset.length());

    glGenTextures(1, &atlas.fontTexture);
    glBindTexture(GL_TEXTURE_2D, atlas.fontTexture);

    // ✅ CRITICAL FIX: Set pixel unpack alignment to 1 byte
    // By default, OpenGL expects rows to be aligned to 4 bytes
    // With GL_ALPHA (1 byte per pixel), widths not divisible by 4 get skewed
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Upload as ALPHA texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
        atlas.textureWidth, atlas.textureHeight,
        0, GL_ALPHA, GL_UNSIGNED_BYTE, atlas.pixels);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("❌ glTexImage2D error: 0x%x", err);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    LOGD("✓ Font texture uploaded successfully! Texture ID: %d", atlas.fontTexture);
    
    if (g_fontShader == 0) {
        initFontRenderer();
    }
}

void drawText(const std::string& text,
              float x, float y,
              float r, float g, float b, float a,
              FontAtlas& atlas, ui::TextView* textView)
{
    if (text.empty()) return;
    
    if (!atlas.pixels) {
        LOGE("❌ drawText: pixels is NULL!");
        return;
    }
    
    if (atlas.glyphX.empty() || atlas.glyphW.empty()) {
        LOGE("❌ drawText: glyph data empty!");
        return;
    }
    
    if (atlas.fontTexture == 0) uploadFontTexture(atlas);
    
    if (g_fontShader == 0) {
        LOGE("❌ drawText: shader is 0!");
        return;
    }
    
    if (atlas.fontTexture == 0) {
        LOGE("❌ drawText: texture is 0!");
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(g_fontShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas.fontTexture);
    glUniform1i(g_uTex, 0);
    glUniform4f(g_uColor, r, g, b, a);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float vw = float(viewport[2]);
    float vh = float(viewport[3]);

    std::vector<GLfloat> vertices;
    std::vector<GLushort> indices;
    vertices.reserve(text.length() * 16);
    indices.reserve(text.length() * 6);

    float startX = x;
    float cx = x;
    float cy = y;
    GLushort vertexIndex = 0;

    float fontHeight = float(atlas.textureHeight);
    float texWidth = float(atlas.textureWidth);
    float texHeight = float(atlas.textureHeight);

    auto [textWidth, textHeight] = textView->getSize();
    float maxX = x + textWidth;
    
    // Helper to get character width
    auto getCharWidth = [&](char c) -> float {
        if (c == ' ') return atlas.fontSize * 0.3f;
        if (c == '\n') return 0.0f;
        
        size_t idx = atlas.charset.find(c);
        if (idx != std::string::npos) {
            return float(atlas.glyphW[idx]);
        }
        return atlas.fontSize * 0.5f;
    };

    // First pass: calculate word boundaries
    std::vector<size_t> wordStarts;
    std::vector<float> wordWidths;
    size_t wordStart = 0;
    float currentWordWidth = 0.0f;
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        float charWidth = getCharWidth(c);
        
        if (c == ' ' || c == '\n') {
            if (currentWordWidth > 0) {
                wordStarts.push_back(wordStart);
                wordWidths.push_back(currentWordWidth);
            }
            
            wordStart = i + 1;
            currentWordWidth = 0.0f;
            
            if (c == '\n') {
                wordStarts.push_back(i);
                wordWidths.push_back(0.0f);
            }
        } 
        else if (i == text.length() - 1) {
            currentWordWidth += charWidth;
            wordStarts.push_back(wordStart);
            wordWidths.push_back(currentWordWidth);
        }
        else {
            currentWordWidth += charWidth;
        }
    }
    
    // Second pass: render with word wrapping
    size_t currentWordIdx = 0;
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        
        // Check if we're at the start of a word
        if (currentWordIdx < wordStarts.size() && i == wordStarts[currentWordIdx]) {
            float wordWidth = wordWidths[currentWordIdx];
            
            // Word wrap: if word doesn't fit and we're not at line start
            if (cx + wordWidth > maxX && cx > startX) {
                cx = startX;
                cy += fontHeight;
            }
            
            currentWordIdx++;
        }
        
        // Handle newlines
        if (c == '\n') {
            cx = startX;
            cy += fontHeight;
            continue;
        }
        
        // Handle spaces
        if (c == ' ') {
            cx += getCharWidth(c);
            continue;
        }
        
        // Skip characters not in charset
        size_t idx = atlas.charset.find(c);
        if (idx == std::string::npos) {
            cx += getCharWidth(c);
            continue;
        }
        
        float gw = float(atlas.glyphW[idx]);
        float gx = float(atlas.glyphX[idx]);
        
        // Viewport culling
        if (cy + fontHeight < -200) continue; 
        if (cy >= vh + 200) break;
        if (cx + gw < -100) {
            cx += gw;
            continue;
        }
        if (cx >= vw + 100) {
            cx += gw;
            continue;
        }

        // Calculate texture coordinates
        float tx0 = gx / texWidth;
        float tx1 = (gx + gw) / texWidth;
        float ty0 = 0.0f;
        float ty1 = 1.0f;

        // Screen quad in pixels
        float x0 = cx;
        float y0 = cy;
        float x1 = cx + gw;
        float y1 = cy + fontHeight;

        // Convert to NDC
        float ndcX0 = (x0 / vw) * 2.0f - 1.0f;
        float ndcY0 = 1.0f - (y0 / vh) * 2.0f;
        float ndcX1 = (x1 / vw) * 2.0f - 1.0f;
        float ndcY1 = 1.0f - (y1 / vh) * 2.0f;

        // Add vertices (position + texcoord interleaved)
        vertices.push_back(ndcX0); vertices.push_back(ndcY1);
        vertices.push_back(tx0); vertices.push_back(ty1);
        
        vertices.push_back(ndcX1); vertices.push_back(ndcY1);
        vertices.push_back(tx1); vertices.push_back(ty1);
        
        vertices.push_back(ndcX1); vertices.push_back(ndcY0);
        vertices.push_back(tx1); vertices.push_back(ty0);
        
        vertices.push_back(ndcX0); vertices.push_back(ndcY0);
        vertices.push_back(tx0); vertices.push_back(ty0);

        // Add indices (two triangles per character)
        indices.push_back(vertexIndex + 0);
        indices.push_back(vertexIndex + 1);
        indices.push_back(vertexIndex + 2);
        
        indices.push_back(vertexIndex + 0);
        indices.push_back(vertexIndex + 2);
        indices.push_back(vertexIndex + 3);

        vertexIndex += 4;
        cx += gw;
    }

    if (vertices.empty()) return;

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort),
                 indices.data(), GL_DYNAMIC_DRAW);

    // Set vertex attributes
    glEnableVertexAttribArray(g_aPos);
    glEnableVertexAttribArray(g_aTex);

    glVertexAttribPointer(g_aPos, 2, GL_FLOAT, GL_FALSE, 
                         4 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(g_aTex, 2, GL_FLOAT, GL_FALSE, 
                         4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    // Draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("OpenGL error after draw: 0x%x", err);
    }

    // Cleanup
    glDisableVertexAttribArray(g_aPos);
    glDisableVertexAttribArray(g_aTex);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

float measureText(const std::string& text, const FontAtlas& atlas) {
    if (text.empty() || atlas.glyphW.empty()) return 0.0f;
    
    float width = 0.0f;
    for (char c : text) {
        size_t idx = atlas.charset.find(c);
        if (idx != std::string::npos && idx < atlas.glyphW.size()) {
            width += float(atlas.glyphW[idx]);
        }
    }
    return width;
}

TextMetrics measureTextWrapped(const std::string& text, float maxWidth, const FontAtlas& atlas) {
    if (text.empty() || atlas.glyphW.empty()) {
        return {0.0f, 0.0f, 0};
    }
    
    float fontHeight = float(atlas.textureHeight);
    
    // Helper to calculate character width
    auto getCharWidth = [&](char c) -> float {
        if (c == ' ') return atlas.fontSize * 0.3f;
        if (c == '\n') return 0.0f;
        
        size_t idx = atlas.charset.find(c);
        if (idx != std::string::npos) {
            return float(atlas.glyphW[idx]);
        }
        return atlas.fontSize * 0.5f;
    };
    
    // ✅ FIRST: Calculate actual text width without any wrapping
    float actualWidth = 0.0f;
    int newlineCount = 0;
    float currentLineWidth = 0.0f;
    float maxActualLineWidth = 0.0f;
    
    for (char c : text) {
        if (c == '\n') {
            maxActualLineWidth = std::max(maxActualLineWidth, currentLineWidth);
            currentLineWidth = 0.0f;
            newlineCount++;
        } else {
            currentLineWidth += getCharWidth(c);
        }
    }
    maxActualLineWidth = std::max(maxActualLineWidth, currentLineWidth);
    actualWidth = maxActualLineWidth;
    
    // ✅ If no maxWidth specified OR text fits without wrapping, return actual size
    if (maxWidth <= 0.0f || actualWidth <= maxWidth) {
        return {actualWidth, fontHeight * (newlineCount + 1), newlineCount + 1};
    }
    
    // ✅ Text needs wrapping - simulate word-wrapped layout
    float startX = 0.0f;
    float cx = startX;
    float cy = 0.0f;
    float maxLineWidth = 0.0f;
    
    // First pass: calculate word boundaries
    std::vector<size_t> wordStarts;
    std::vector<float> wordWidths;
    size_t wordStart = 0;
    float currentWordWidth = 0.0f;
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        float charWidth = getCharWidth(c);
        
        if (c == ' ' || c == '\n') {
            if (currentWordWidth > 0) {
                wordStarts.push_back(wordStart);
                wordWidths.push_back(currentWordWidth);
            }
            
            wordStart = i + 1;
            currentWordWidth = 0.0f;
            
            if (c == '\n') {
                wordStarts.push_back(i);
                wordWidths.push_back(0.0f);  // Marker for explicit line break
            }
        } 
        else if (i == text.length() - 1) {
            currentWordWidth += charWidth;
            wordStarts.push_back(wordStart);
            wordWidths.push_back(currentWordWidth);
        }
        else {
            currentWordWidth += charWidth;
        }
    }
    
    // Second pass: simulate layout with word wrapping
    size_t currentWordIdx = 0;
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        
        // At start of a word
        if (currentWordIdx < wordStarts.size() && i == wordStarts[currentWordIdx]) {
            float wordWidth = wordWidths[currentWordIdx];
            
            // Wrap if word doesn't fit and we're not at line start
            if (cx + wordWidth > maxWidth && cx > startX) {
                maxLineWidth = std::max(maxLineWidth, cx);
                cx = startX;
                cy += fontHeight;
            }
            
            currentWordIdx++;
        }
        
        // Handle newlines
        if (c == '\n') {
            maxLineWidth = std::max(maxLineWidth, cx);
            cx = startX;
            cy += fontHeight;
            continue;
        }
        
        // Add character width
        cx += getCharWidth(c);
    }
    
    // Record final line width
    maxLineWidth = std::max(maxLineWidth, cx);
    
    // Calculate total height
    float totalHeight = cy + fontHeight;
    int lineCount = int(cy / fontHeight) + 1;
    
    return {maxLineWidth, totalHeight, lineCount};
}

float getFontHeight(const FontAtlas& atlas) {
    return float(atlas.textureHeight);
}