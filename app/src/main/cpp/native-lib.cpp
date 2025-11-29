#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include "ui/View.h"
#include "input/Dispatcher.h"


static GLint width, height;
static GLint m_viewport[4];

static ui::View* rootView = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeInit(JNIEnv*, jclass) {
    glGetIntegerv( GL_VIEWPORT, m_viewport );
    width = m_viewport[2];
    height = m_viewport[3];
    
    if (rootView) return;

    rootView = new ui::View();
    ui::View* titleBar = new ui::View();
    input::setRoot(rootView);
    
    titleBar->setPosition(0, 0);
    titleBar->setViewport(width, height);
    titleBar->setSize(width, height/12);
    titleBar->setColor(0.87f, 0.1f, 0.3f, 1.0f);

    titleBar->setOnTouchDownListener([=](float x, float y){
        titleBar->setColor(0.85f, 0.08f, 0.28f, 1.0f);
    });
    titleBar->setOnTouchUpListener([=](float x, float y){
        titleBar->setColor(0.87f, 0.1f, 0.3f, 1.0f);
    });

    rootView->setViewport(width, height);
    rootView->setPosition(0, 0);
    rootView->setColor(0.9f, 0.9f, 0.9f, 1.0f);
    rootView->setSize(width, height);
    rootView->addChild(titleBar);
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
    if (rootView) rootView->draw();
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
