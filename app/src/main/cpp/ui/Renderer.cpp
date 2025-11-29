#include "ui/Renderer.h"

static GLuint rectProgram = 0;
static GLint posLoc = -1;
static GLint colorLoc = -1;
static GLint sizeLoc = -1;
static GLint offsetLoc = -1;
static GLint scrollLoc = -1;

void initRectShader()
{
    destroyGLResources(); // safety, if context was lost

    const char* vsSrc = "attribute vec2 aPos;"
        "uniform vec2 uOffset;"
        "uniform vec2 uSize;"
        "uniform float uScroll;"
        "void main() {"
        " vec2 pos = (aPos * uSize) + vec2(uOffset.x, uOffset.y + uScroll);"
        " gl_Position = vec4(pos, 0.0, 1.0);"
        "}";

    const char* fsSrc = "precision mediump float;"
        "uniform vec4 uColor;"
        "void main() { gl_FragColor = uColor; }";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSrc, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, nullptr);
    glCompileShader(fs);

    rectProgram = glCreateProgram();
    glAttachShader(rectProgram, vs);
    glAttachShader(rectProgram, fs);
    glLinkProgram(rectProgram);

    posLoc    = glGetAttribLocation(rectProgram, "aPos");
    offsetLoc = glGetUniformLocation(rectProgram, "uOffset");
    sizeLoc   = glGetUniformLocation(rectProgram, "uSize");
    colorLoc  = glGetUniformLocation(rectProgram, "uColor");
    scrollLoc = glGetUniformLocation(rectProgram, "uScroll");

    glDeleteShader(vs); // optional cleanup
    glDeleteShader(fs);
}

void destroyGLResources()
{
    if (rectProgram != 0) {
        glDeleteProgram(rectProgram);
        rectProgram = 0;
        posLoc = colorLoc = sizeLoc = offsetLoc = scrollLoc = -1;
    }
}

// Accessors
GLuint getRectProgram() { return rectProgram; }
GLint getPosLoc() { return posLoc; }
GLint getOffsetLoc() { return offsetLoc; }
GLint getSizeLoc() { return sizeLoc; }
GLint getColorLoc() { return colorLoc; }
GLint getScrollLoc() { return scrollLoc; }
