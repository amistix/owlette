#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include "ui/View.h"
#include "ui/ScrollView.h"
#include "input/Dispatcher.h"
#include "ui/TextView.h"
#include <android/log.h>
#include "ui/FontRenderer.h"
#include "ui/EditTextView.h"


static GLint width, height;
static GLint m_viewport[4];
JavaVM* g_vm = nullptr;
jobject g_activity = nullptr;

static ui::View* rootView = nullptr;

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
Java_org_amistix_owlette_MainActivity_nativeGetMainActivity(
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

        ui::EditTextView* content = new ui::EditTextView();
        content->setPosition(50, 50);
        content->setSize(400, 200);
        content->setText("Hello, Owlette!");
        content->setColor(0.0f, 0.0f, 0.0f, 0.0f);
        content->setColorText(0.0f, 0.0f, 0.0f, 1.0f);

        placeholder->addChild(content);
        scrollView->addChild(placeholder);

        placeholder->setOnTouchDownListener([placeholder, scrollView, content](float x, float y){
            scrollView->focus(x, y);
            content->setFocused(true);
            placeholder->setColor(0.7f, 0.7f, 0.7f, 1.0f);
        });

        placeholder->setOnTouchMoveListener([scrollView](float x, float y){
            scrollView->scroll(x, y);
        });

        placeholder->setOnTouchUpListener([placeholder, scrollView](float x, float y){
            scrollView->unfocus(x, y);
            placeholder->setColor(0.5f, 0.5f, 0.5f, 1.0f);
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
Java_org_amistix_owlette_GLView_nativeDestroy(JNIEnv* env, jclass) {
    if (rootView) 
    {
        rootView->destroy(); 
        delete rootView;
        rootView = nullptr;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeTouch(
    JNIEnv*, jclass, jint action, jfloat x, jfloat y) {
    input::handleTouch(action, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_MainActivity_nativeOnTextChanged(
    JNIEnv* env, jobject obj, jstring text) {
    const char* str = env->GetStringUTFChars(text, nullptr);
    ui::EditTextView* focused = ui::EditTextView::getFocusedInstance();
    if (focused) {
        focused->updateText(std::string(str));
    }
    
    env->ReleaseStringUTFChars(text, str);
}

