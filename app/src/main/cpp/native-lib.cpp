#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include <cmath>
#include <chrono>
#include "ui/View.h"

static GLuint program;
static GLint posAttr;

static GLint offsetXLoc;
static GLint offsetYLoc;

static GLint scaleLoc;
static GLint colorLoc;

static auto startTime = std::chrono::steady_clock::now();

static GLint width, height;
static GLint m_viewport[4];

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeInit(JNIEnv*, jclass) {
    const char* vShaderSrc =
        "attribute vec2 vPosition;\n"
        "uniform float uOffsetX;\n"
        "uniform float uOffsetY;\n"
        "uniform float uScale;\n"
        "void main() {\n"
        "  vec2 pos = vPosition * uScale + vec2(uOffsetX, uOffsetY);\n"
        "  gl_Position = vec4(pos, 0.0, 1.0);\n"
        "}";

    const char* fShaderSrc =
        "precision mediump float;\n"
        "uniform vec4 uColor;\n"
        "void main() { gl_FragColor = uColor; }";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShaderSrc, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShaderSrc, nullptr);
    glCompileShader(fs);

    glGetIntegerv( GL_VIEWPORT, m_viewport );
    width = m_viewport[2];
    height = m_viewport[3];

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    posAttr = glGetAttribLocation(program, "vPosition");
    offsetXLoc = glGetUniformLocation(program, "uOffsetX");
    offsetYLoc = glGetUniformLocation(program, "uOffsetY");
    scaleLoc = glGetUniformLocation(program, "uScale");
    colorLoc = glGetUniformLocation(program, "uColor");

    startTime = std::chrono::steady_clock::now();
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeResize(JNIEnv*, jclass, jint width, jint height) {
    glViewport(0, 0, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDraw(JNIEnv*, jclass) {
    using namespace std::chrono;
    float t = duration<float>(steady_clock::now() - startTime).count();

    float offsetX = std::sin(t) * 0.3f * height / width;
    float offsetY = std::cos(t) * 0.3f;
    float scale  = std::sin(t * 5.0f) * 0.5f + 0.2f;

    glClearColor(offsetX, offsetY, scale, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glUniform1f(offsetXLoc, offsetX);
    glUniform1f(offsetYLoc, offsetY);
    glUniform1f(scaleLoc, scale);
    glUniform4f(colorLoc, std::sin(t) * 0.5f + 0.5f,
                         std::cos(t) * 0.5f + 0.5f,
                         0.3f, 1.0f);

    GLfloat vertices[] = {
        -0.3f * height / width, -0.3f,
         0.3f * height / width, -0.3f,
        -0.3f * height / width,  0.3f,
         0.3f * height / width,  0.3f
    };

    glEnableVertexAttribArray(posAttr);
    glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(posAttr);
}
