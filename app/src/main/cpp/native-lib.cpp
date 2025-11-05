#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>

#include <cmath>
#include <ctime>

static GLuint program;
static GLint posAttr;
static float timeStart = 0.0f;

static GLint offsetLoc;
static GLint scaleLoc;
static GLint colorLoc;


extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_MainActivity_nativeInit(JNIEnv*, jclass) {
    const char* vShaderSrc =
        "attribute vec2 vPosition;\n"
        "uniform float uOffset;\n"
        "uniform float uScale;\n"
        "void main() {\n"
        "  vec2 pos = vPosition * uScale + vec2(uOffset, 0.0);\n"
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

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    posAttr = glGetAttribLocation(program, "vPosition");
    offsetLoc = glGetUniformLocation(program, "uOffset");
    scaleLoc = glGetUniformLocation(program, "uScale");
    colorLoc = glGetUniformLocation(program, "uColor");
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_MainActivity_nativeResize(JNIEnv*, jclass, jint width, jint height) {
    glViewport(0, 0, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_MainActivity_nativeDraw(JNIEnv*, jclass) {
    float currentTime = static_cast<float>(clock()) / CLOCKS_PER_SEC;
    float t = currentTime - timeStart;

    float offset = std::sin(t);      
    float scale = 0.6f + std::sin(t * 2.0f) * 0.5f;

    // Set uniforms
    
    glUniform1f(offsetLoc, offset);
    glUniform1f(scaleLoc, scale);

    glUniform4f(colorLoc, (std::sin(t) * 0.5f + 0.5f), (std::cos(t) * 0.5f + 0.5f), 0.3f, 1.0f);

    glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat vertices[] = {
        -0.3f, -0.3f,
         0.3f, -0.3f,
        -0.3f,  0.3f,
         0.3f,  0.3f
    };

    glUseProgram(program);
    glEnableVertexAttribArray(posAttr);
    glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(posAttr);
}