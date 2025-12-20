#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <jni.h>
#include <android/log.h>

#include "input/Dispatcher.h"
#include "ui/FontRenderer.h"

#include "ui/EditTextView.h"

#include "main.h"

GLint width, height;
static GLint m_viewport[4];
JavaVM* g_vm = nullptr;
jobject g_activity = nullptr;

static JNIEnv* getEnv() {
    JNIEnv* env = nullptr;

    // Check if the thread already has JNIEnv
    jint result = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (result == JNI_EDETACHED) {
        // Attach current thread
        g_vm->AttachCurrentThread(&env, nullptr);
    }

    return env;
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_KeyboardTriggerer_nativeGetKeyboardTriggerer(
    JNIEnv* env, jobject obj) {

    env->GetJavaVM(&g_vm);
    if (g_activity) {
        env->DeleteGlobalRef(g_activity);
    }
    g_activity = env->NewGlobalRef(obj);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeInit(JNIEnv* env, jobject obj) {

    glGetIntegerv( GL_VIEWPORT, m_viewport );
    width = m_viewport[2];
    height = m_viewport[3];

    destroyGLResources();
    initRectShader();
    initFontRenderer();
    uploadFontTexture(); 
    
    onInit();
    input::setRoot(getRootView());
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeResize(JNIEnv*, jclass, jint width, jint height) {
    glViewport(0, 0, width, height);
    onResize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDraw(JNIEnv*, jclass) {
    onDraw();
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDestroy(JNIEnv* env, jclass) {
    onDestroy();
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeTouch(
    JNIEnv*, jclass, jint action, jfloat x, jfloat y) {
    input::handleTouch(action, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_KeyboardTriggerer_nativeOnTextChanged(
    JNIEnv* env, jobject obj, jstring text) {
    const char* str = env->GetStringUTFChars(text, nullptr);
    ui::EditTextView* focused = ui::EditTextView::getFocusedInstance();
    if (focused) {
        focused->updateText(std::string(str));
    }
    
    env->ReleaseStringUTFChars(text, str);
}

