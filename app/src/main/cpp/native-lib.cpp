#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include "ui/View.h"


static GLint width, height;
static GLint m_viewport[4];

static ui::View* rootView = nullptr;
static ui::View* titleBar = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeInit(JNIEnv*, jclass) {
    glGetIntegerv( GL_VIEWPORT, m_viewport );
    width = m_viewport[2];
    height = m_viewport[3];
    
    
    
    if (!rootView) rootView = new ui::View();
    if (!titleBar) titleBar = new ui::View();
    
    titleBar->setPosition(0, 0);
    titleBar->setViewport(width, height);
    titleBar->setSize(width, height/12);
    titleBar->setColor(0.87f, 0.1f, 0.3f, 1.0f);

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
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  
    glClear(GL_COLOR_BUFFER_BIT);

    if (rootView) rootView->draw();
        
}

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeDestroy(JNIEnv*, jclass) {
    if (rootView) rootView->destroy(); 
}
