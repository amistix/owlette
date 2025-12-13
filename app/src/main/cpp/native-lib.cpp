#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include "ui/View.h"
#include "ui/ScrollView.h"
#include "input/Dispatcher.h"
#include "ui/TextView.h"
#include <android/log.h>
// #include "ui/EditTextView.h"

// #include "input/InputManager.h"
#include "ui/FontRenderer.h"

static GLint width, height;
static GLint m_viewport[4];

static JavaVM* g_vm;

static ui::View* rootView = nullptr;
void openKeyboard();

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeInit(JNIEnv* env, jclass) {

    env->GetJavaVM(&g_vm);

    glGetIntegerv( GL_VIEWPORT, m_viewport );
    width = m_viewport[2];
    height = m_viewport[3];

    destroyGLResources();
    initRectShader();
    initFontRenderer();
    uploadFontTexture(); 
    
    if (rootView) return;

    rootView = new ui::View();
    input::setRoot(rootView);

    rootView->setViewport(width, height);
    rootView->setPosition(0, 0);
    rootView->setColor(0.9f, 0.9f, 0.9f, 1.0f);
    rootView->setSize(width, height);

    ui::ScrollView* scrollView = new ui::ScrollView();
    scrollView->setPosition(0, 0);
    scrollView->setSize(width, height);
    scrollView->setColor(0.8f, 0.8f, 0.8f, 0.0f);
    scrollView->setContainerHeight(4210);

    for (int i=0; i<20; i++)
    {

        ui::View* placeholder = new ui::View();
        placeholder->setPosition(10, 210 * i + 10);
        placeholder->setSize(width - 20, 200);
        placeholder->setColor(0.5f, 0.5f, 0.5f, 1.0f);

        ui::TextView* content = new ui::TextView();
        content->setPosition(50, 50);
        content->setSize(400, 200);
        content->setText("Hello, Owlette!");
        content->setColor(0.0f, 0.0f, 0.0f, 0.0f);
        content->setColorText(0.0f, 0.0f, 0.0f, 1.0f);

        content->setOnTouchDownListener([=](float x, float y){
            // openKeyboard();
            content->getParent()->onTouchDown(x, y);
        });

        content->setOnTouchMoveListener([=](float x, float y){
            content->getParent()->onTouchMove(x, y);
        });

        content->setOnTouchUpListener([=](float x, float y){
            // openKeyboard();
            content->getParent()->onTouchUp(x, y);
        });

        placeholder->addChild(content);
        scrollView->addChild(placeholder);

        content->getParent()->setOnTouchDownListener([=](float x, float y){
            // openKeyboard();
            scrollView->focus(x, y);
            content->getParent()->setColor(0.7f, 0.7f, 0.7f, 1.0f);
        });

        content->getParent()->setOnTouchMoveListener([=](float x, float y){
            scrollView->scroll(x, y);
        });

        content->getParent()->setOnTouchUpListener([=](float x, float y){
            // openKeyboard();
            scrollView->unfocus(x, y);
            content->getParent()->setColor(0.5f, 0.5f, 0.5f, 1.0f);
        });
    }

    rootView->addChild(scrollView);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeResize(JNIEnv*, jclass, jint width, jint height) {
    glViewport(0, 0, width, height);
    if (rootView)
    {
        rootView->setSize(width, height);
        rootView->setViewport(width, height);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDraw(JNIEnv*, jclass) {
    if (rootView) 
    {
        rootView->draw();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDestroy(JNIEnv*, jclass) {
    if (rootView) rootView->destroy(); 
    delete rootView;
    rootView = nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeTouch(JNIEnv*, jclass, jint action, jfloat x, jfloat y)
{
    input::handleTouch(action, x, y);
}

JNIEnv* getEnv() {
    JNIEnv* env = nullptr;

    // Check if the thread already has JNIEnv
    jint result = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (result == JNI_EDETACHED) {
        // Attach current thread
        g_vm->AttachCurrentThread(&env, nullptr);
    }

    return env;
}

void openKeyboard() {
    JNIEnv* env = getEnv();
    // jclass cls = env->FindClass("org/amistix/owlette/MainActivity");
    // jmethodID mid = env->GetStaticMethodID(cls, "showKeyboardFromNative", "()V");
    // env->CallStaticVoidMethod(cls, mid);
}