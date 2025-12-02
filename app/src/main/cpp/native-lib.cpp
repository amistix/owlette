#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include "ui/View.h"
#include "ui/ScrollView.h"
#include "input/Dispatcher.h"


static GLint width, height;
static GLint m_viewport[4];

static ui::View* rootView = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_org_amistix_owlette_GLView_nativeInit(JNIEnv*, jclass) {
    glGetIntegerv( GL_VIEWPORT, m_viewport );
    width = m_viewport[2];
    height = m_viewport[3];

    destroyGLResources();
    initRectShader();
    
    if (rootView) return;

    rootView = new ui::View();
    input::setRoot(rootView);

    rootView->setViewport(width, height);
    rootView->setPosition(0, 0);
    rootView->setColor(0.9f, 0.9f, 0.9f, 1.0f);
    rootView->setSize(width, height);

    ui::ScrollView* content = new ui::ScrollView();
    content->setPosition(0, 0);
    content->setSize(width, height);
    content->setColor(0.95f, 0.95f, 0.95f, 1.0f);
    content->setContainerHeight(3000.0f);

    for (int i = 0; i < 20; i++) {
        ui::View* item = new ui::View();
        item->setSize(width, 150);
        item->setPosition(0, i * 150);
        item->setColor(0.2f, 0.2f + i*0.02f, 0.35f, 1.0f);

        item->setOnTouchDownListener([=](float x, float y){
            content->focus(x, y);
            item->setSize(width * 3 / 4, 150);
        });
        item->setOnTouchMoveListener([=](float x, float y){
            content->scroll(x, y);
        });
        item->setOnTouchUpListener([=](float x, float y){
            content->unfocus(x, y);
            item->setSize(width, 150);
        });

        content->addChild(item);
    }

    rootView->addChild(content);
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
