#include <EGL/egl.h>
#include <jni.h>
#include <GLES2/gl2.h>
#include "ui/View.h"
#include "ui/ScrollView.h"
#include "input/Dispatcher.h"
#include "ui/TextView.h"

#include <chrono>
#include <cmath>

static GLint width, height;
static GLint m_viewport[4];

static ui::View* rootView = nullptr;
static ui::TextView* content = nullptr;

static std::chrono::time_point<std::chrono::steady_clock> start_time;
static std::chrono::time_point<std::chrono::steady_clock> stop_time;

static bool is_stopped = false;

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

    start_time = std::chrono::steady_clock::now();

    rootView->setViewport(width, height);
    rootView->setPosition(0, 0);
    rootView->setColor(0.9f, 0.9f, 0.9f, 1.0f);
    rootView->setSize(width, height);

    content = new ui::TextView();
    content->setPosition(width / 8, height / 2 - 100);
    content->setSize(6 * width / 8, 200);
    content->setText("Hello, Owlette!");
    content->setColor(0.5f, 0.5f, 0.5f, 1.0f);
    content->setColorText(0.2f, 0.2f, 0.8f, 1.0f);

    rootView->addChild(content);

    content->setOnTouchDownListener([=](float x, float y){
        rootView->setColor(0.7f, 0.7f, 0.7f, 1.0f);
        is_stopped = true;
        stop_time = std::chrono::steady_clock::now();
    });

    content->setOnTouchUpListener([=](float x, float y){
        rootView->setColor(0.9f, 0.9f, 0.9f, 1.0f);
        is_stopped = false;
        std::chrono::duration<long double> stop_duration_in_seconds = std::chrono::steady_clock::now() - stop_time;
        std::chrono::seconds seconds_offset = std::chrono::duration_cast<std::chrono::seconds>(stop_duration_in_seconds);
        start_time += seconds_offset;
    });
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

        std::chrono::duration<long double> duration_in_seconds = std::chrono::steady_clock::now() - start_time;
        content->setColor(0.5f + 0.5f * std::sin(duration_in_seconds.count()), 
                          0.5f + 0.5f * std::sin(duration_in_seconds.count() + 2.0f), 
                          0.5f + 0.5f * std::sin(duration_in_seconds.count() + 4.0f), 
                          1.0f);
        content->setColorText(0.5f + 0.1f * std::sin(duration_in_seconds.count()), 
                             0.1f + 0.5f * std::sin(duration_in_seconds.count()), 
                             0.3f + 0.2f * std::sin(duration_in_seconds.count()), 
                             1.0f);
        if (is_stopped) return;
         content->setPosition(width / 8, height / 2 -100 + std::sin(duration_in_seconds.count()) * 400);
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
