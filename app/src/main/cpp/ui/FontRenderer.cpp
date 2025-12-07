#include "ui/FontRenderer.h"
#include <GLES2/gl2.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <cstdio>

#define LOG_TAG "FontRenderer"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// External font data from JNI
extern int g_fontWidth;
extern int g_fontHeight;
extern unsigned char* g_fontPixels;
extern std::vector<int> g_glyphX;
extern std::vector<int> g_glyphW;
extern std::string g_charset;

// OpenGL resources
static GLuint g_fontTexture = 0;
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
    float alpha = texture2D(uTex, vTex).r;
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
    if (g_fontShader != 0) return; // Already initialized
    
    LOGD("Initializing font renderer");
    
    // Create shader program
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

void cleanupFontRenderer() {
    if (g_fontTexture != 0) {
        glDeleteTextures(1, &g_fontTexture);
        g_fontTexture = 0;
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

void uploadFontTexture() {
    if (g_fontTexture != 0) return; // Already uploaded
    
    if (!g_fontPixels) {
        LOGE("❌ Font pixels not loaded! g_fontPixels is NULL");
        return;
    }
    
    if (g_fontWidth == 0 || g_fontHeight == 0) {
        LOGE("❌ Invalid font dimensions: %dx%d", g_fontWidth, g_fontHeight);
        return;
    }
    
    if (g_glyphX.empty() || g_glyphW.empty()) {
        LOGE("❌ Glyph data not loaded! glyphX size:%zu glyphW size:%zu", 
             g_glyphX.size(), g_glyphW.size());
        return;
    }
    
    if (g_charset.empty()) {
        LOGE("❌ Charset is empty!");
        return;
    }

    LOGD("✓ Creating font texture...");
    LOGD("  - Dimensions: %dx%d", g_fontWidth, g_fontHeight);
    LOGD("  - Glyphs: %zu", g_glyphX.size());
    LOGD("  - Charset: %s", g_charset.c_str());

    glGenTextures(1, &g_fontTexture);
    glBindTexture(GL_TEXTURE_2D, g_fontTexture);

    // Use GL_LUMINANCE (single channel) for ALPHA_8 bitmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                 g_fontWidth, g_fontHeight,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, g_fontPixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    LOGD("✓ Font texture uploaded successfully! Texture ID: %d", g_fontTexture);
    
    // Initialize shader if not done yet
    if (g_fontShader == 0) {
        initFontRenderer();
    }
}

void drawText(const std::string& text,
              float x, float y,
              float r, float g, float b, float a,
              float size)
{
    // Debug checks
    if (text.empty()) {
        LOGD("drawText: empty text");
        return;
    }
    
    if (!g_fontPixels) {
        LOGE("❌ drawText: g_fontPixels is NULL! Font not loaded.");
        return;
    }
    
    if (g_glyphX.empty() || g_glyphW.empty()) {
        LOGE("❌ drawText: glyph data empty! glyphX:%zu glyphW:%zu", 
             g_glyphX.size(), g_glyphW.size());
        return;
    }
    
    // Ensure texture and shader are ready
    uploadFontTexture();
    
    if (g_fontShader == 0) {
        LOGE("❌ drawText: shader program is 0!");
        return;
    }
    
    if (g_fontTexture == 0) {
        LOGE("❌ drawText: font texture is 0!");
        return;
    }

    LOGD("✓ Drawing text: '%s' at (%.1f, %.1f) color:(%.2f,%.2f,%.2f,%.2f)", 
         text.c_str(), x, y, r, g, b, a);

    // Enable blending for transparent text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use shader program
    glUseProgram(g_fontShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_fontTexture);
    glUniform1i(g_uTex, 0);
    glUniform4f(g_uColor, r, g, b, a);

    // Get viewport for NDC conversion
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float vw = float(viewport[2]);
    float vh = float(viewport[3]);

    LOGD("  Viewport: %dx%d", viewport[2], viewport[3]);

    // Build vertex data for all characters
    std::vector<GLfloat> vertices;
    std::vector<GLushort> indices;
    vertices.reserve(text.length() * 16); // 4 vertices * 4 floats per char
    indices.reserve(text.length() * 6);   // 6 indices per char

    float cx = x;
    float cy = y;
    GLushort vertexIndex = 0;
    int charCount = 0;

    for (char c : text) {
        size_t idx = g_charset.find(c);
        if (idx == std::string::npos) {
            LOGD("  Character '%c' not in charset, skipping", c);
            cx += size * 0.5f;
            continue;
        }

        float gw = float(g_glyphW[idx]);
        float gx = float(g_glyphX[idx]);

        LOGD("  Char '%c': glyph x=%.1f w=%.1f", c, gx, gw);

        // Texture coordinates
        float tx0 = gx / float(g_fontWidth);
        float tx1 = (gx + gw) / float(g_fontWidth);
        float ty0 = 0.0f;
        float ty1 = 1.0f;

        // Convert pixel coordinates to NDC [-1, 1]
        float ndcX0 = (cx / vw) * 2.0f - 1.0f;
        float ndcY0 = 1.0f - (cy / vh) * 2.0f;
        float ndcX1 = ((cx + gw) / vw) * 2.0f - 1.0f;
        float ndcY1 = 1.0f - ((cy + g_fontHeight) / vh) * 2.0f;

        // Add 4 vertices (position + texcoord)
        vertices.push_back(ndcX0); vertices.push_back(ndcY1);
        vertices.push_back(tx0); vertices.push_back(ty1);
        
        vertices.push_back(ndcX1); vertices.push_back(ndcY1);
        vertices.push_back(tx1); vertices.push_back(ty1);
        
        vertices.push_back(ndcX1); vertices.push_back(ndcY0);
        vertices.push_back(tx1); vertices.push_back(ty0);
        
        vertices.push_back(ndcX0); vertices.push_back(ndcY0);
        vertices.push_back(tx0); vertices.push_back(ty0);

        // Add 6 indices for 2 triangles
        indices.push_back(vertexIndex + 0);
        indices.push_back(vertexIndex + 1);
        indices.push_back(vertexIndex + 2);
        
        indices.push_back(vertexIndex + 0);
        indices.push_back(vertexIndex + 2);
        indices.push_back(vertexIndex + 3);

        vertexIndex += 4;
        cx += gw;
        charCount++;
    }

    if (vertices.empty()) {
        LOGD("  No valid characters to render");
        return;
    }

    LOGD("  Rendering %d characters, %zu vertices, %zu indices", 
         charCount, vertices.size()/4, indices.size());

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(), GL_DYNAMIC_DRAW);

    // Upload index data
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

    // Draw all characters in one call
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

    // Check for GL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("  OpenGL error after draw: 0x%x", err);
    } else {
        LOGD("  ✓ Draw successful!");
    }

    // Cleanup
    glDisableVertexAttribArray(g_aPos);
    glDisableVertexAttribArray(g_aTex);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

float measureText(const std::string& text) {
    if (text.empty() || g_glyphW.empty()) return 0.0f;
    
    float width = 0.0f;
    for (char c : text) {
        size_t idx = g_charset.find(c);
        if (idx != std::string::npos && idx < g_glyphW.size()) {
            width += float(g_glyphW[idx]);
        }
    }
    return width;
}

float getFontHeight() {
    return float(g_fontHeight);
}