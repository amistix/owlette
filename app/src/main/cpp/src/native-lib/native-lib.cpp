#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <jni.h>
#include <android/log.h>

#include "ui/FontRenderer.h"

#include "input/Dispatcher.h"
#include "ui/FontRenderer.h"

#include "storage/AppStorageManager.h"
#include "i2pd/I2pdManager.h"

#include "ui/EditTextView.h"

#include <chrono>
#include "animation/Animator.h"

#include "main.h"

GLint width, height;
static GLint m_viewport[4];
JavaVM* g_vm = nullptr;
jobject g_activity = nullptr;

i2p::I2pdManager* i2pdManager = nullptr;

static auto lastTime = std::chrono::steady_clock::now();

static JNIEnv* getEnv() 
{
    JNIEnv* env = nullptr;

    jint result = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (result == JNI_EDETACHED) g_vm->AttachCurrentThread(&env, nullptr);
    return env;
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_KeyboardTriggerer_nativeGetKeyboardTriggerer(JNIEnv* env, jobject obj) 
{

    env->GetJavaVM(&g_vm);
    if (g_activity) env->DeleteGlobalRef(g_activity);
    
    g_activity = env->NewGlobalRef(obj);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_MainActivity_nativeOnKeyboardVisibilityChanged(
    JNIEnv* env, jobject obj, jint visibleHeight, jint keyboardHeight)
{
    onKeyboardOpen(visibleHeight);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeInit(JNIEnv* env, jobject obj) 
{

    glGetIntegerv(GL_VIEWPORT, m_viewport);
    width = m_viewport[2];
    height = m_viewport[3];

    destroyGLResources();
    initRectShader();
    initFontRenderer();
    
    onInit();
    input::setRoot(getRootView());

    // Re-upload font texture if it exists
    extern FontAtlas* globalAtlas;

    if (globalAtlas && globalAtlas->pixels) 
    {
        globalAtlas->fontTexture = 0;
        uploadFontTexture(*globalAtlas);
    }

    // Initialize the daemon
    if (!i2pdManager) 
    {
        i2pdManager = new i2p::I2pdManager();
        i2pdManager->start();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeResize(JNIEnv*, jclass, jint width, jint height) 
{
    glViewport(0, 0, width, height);
    onResize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDraw(JNIEnv*, jclass) 
{
    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;
    
    animation::Animator::update(deltaTime);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    onDraw();
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_MainActivity_nativeShutdown(JNIEnv* env, jclass)
{
    if (i2pdManager) 
    { 
        i2pdManager->shutdown();
        delete i2pdManager;
        i2pdManager = nullptr;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDestroy(JNIEnv* env, jclass) {onDestroy();}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeTouch(
    JNIEnv*, jclass, jint action, jfloat x, jfloat y) 
{input::handleTouch(action, x, y);}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_KeyboardTriggerer_nativeOnTextChanged(
    JNIEnv* env, jobject obj, jstring text) 
{
    const char* str = env->GetStringUTFChars(text, nullptr);
    ui::EditTextView* focused = ui::EditTextView::getFocusedInstance();
    if (focused) focused->updateText(std::string(str));
    
    env->ReleaseStringUTFChars(text, str);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_AppStorageManager_setAppStoragePath(
    JNIEnv* env, jclass, jstring path) 
{
    const char* str = env->GetStringUTFChars(path, nullptr);
    storage::setAppStoragePath(std::string(str));

    env->ReleaseStringUTFChars(path, str);
}