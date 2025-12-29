#include "main.h"
#include "ui/View.h"
#include "ui/ScrollView.h"
#include "ui/TextView.h"
#include "ui/EditTextView.h"

#include "ui/FontRenderer.h"
#include "ui/Font_JNI.h"

#include <math.h>
#include <chrono>

ui::View* rootView = nullptr;
extern GLint width, height;

FontAtlas* globalAtlas = nullptr;

std::chrono::steady_clock::time_point startTime;

ui::View* getRootView() { return rootView; }

void onInit() 
{
    if (rootView) {
        // Context was lost and recreated - re-upload texture
        if (globalAtlas && globalAtlas->pixels) {
            globalAtlas->fontTexture = 0;
            uploadFontTexture(*globalAtlas);
        }
        return;
    }

    rootView = new ui::View();
    startTime = std::chrono::steady_clock::now();

    rootView->setViewport(width, height);
    rootView->setPosition(0, 0);
    rootView->setColor(0.9f, 0.9f, 0.9f, 1.0f);
    rootView->setSize(width, height);

    globalAtlas = initFont(24.0f);

    
    ui::View* topBar = new ui::View();
    topBar->setPosition(0, 0);
    topBar->setSize(width, 100);
    topBar->setColor(224.0f/255.0f, 25.0f/255.0f, 78.0f/255.0f, 1.0f);
    
    ui::EditTextView* title = new ui::EditTextView();
    title->setPosition(20, 5);
    title->setColor(1.0f, 1.0f, 1.0f, 0.0f);
    title->setColorText(1.0f, 1.0f, 1.0f, 1.0f);
    title->setSize(400, 100);
    title->setText("Owlette UI Demo");
    title->setFontAtlas(globalAtlas);
    title->setOnTouchDownListener([title, topBar](float x, float y) {
        if (!title->isFocused()) 
        title->setFocused(true);
        else 
        title->setFocused(false);
    });
    title->setOnFocusedListener([topBar]() {
        auto currentTime = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(currentTime - startTime).count();
        topBar->setColor(sin(elapsed), sin(elapsed + 1.6), sin(elapsed + 3.14), 1.0f);
    });
    
    
    topBar->addChild(title);
    rootView->addChild(topBar);
    

    ui::ScrollView* scrollView = new ui::ScrollView();
    scrollView->setPosition(0, 110);
    scrollView->setSize(width, height - 120);
    scrollView->setColor(0.8f, 0.8f, 0.8f, 0.0f);
    scrollView->setContainerHeight(210 * 20 + 10);
    for (int i = 0; i < 20; i++)
    {
        ui::View* placeholder = new ui::View();
        placeholder->setPosition(10, 210 * i + 10);
        placeholder->setSize(width - 20, 200);
        placeholder->setColor(0.5f, 0.5f, 0.5f, 1.0f);

        ui::TextView* content = new ui::TextView();
        content->setPosition(50, 50);
        content->setSize(width - 120, 200);
        content->setText("I2P (Invisible Internet Protocol) is a universal anonymous network layer. All communications over I2P are anonymous and end-to-end encrypted, participants don't reveal their real IP addresses. I2P client is a software used for building and using anonymous I2P networks. Such networks are commonly used for anonymous peer-to-peer applications (filesharing, cryptocurrencies) and anonymous client-server applications (websites, instant messengers, chat-servers).");
        content->setColor(0.0f, 0.0f, 0.0f, 0.0f);
        content->setColorText(0.0f, 0.0f, 0.0f, 1.0f);
        content->setFontAtlas(globalAtlas);

        placeholder->addChild(content);
        scrollView->addChild(placeholder);

        placeholder->setOnTouchDownListener([placeholder, scrollView](float x, float y) {
            scrollView->focus(x, y);
            placeholder->setColor(0.7f, 0.7f, 0.7f, 1.0f);
        });

        placeholder->setOnTouchMoveListener([scrollView](float x, float y) {
            scrollView->scroll(x, y);
        });

        placeholder->setOnTouchUpListener([placeholder, scrollView](float x, float y) {
            scrollView->unfocus(x, y);
            placeholder->setColor(0.5f, 0.5f, 0.5f, 1.0f);
        });
    }

    rootView->addChild(scrollView);
}

void onResize(int w, int h) 
{
    width = w;
    height = h;
    
    if (rootView)
    {
        rootView->setSize(width, height);
        rootView->setViewport(width, height);
    }
}

void onDraw() 
{
    if (rootView) {
        rootView->draw();
    }
}

void onDestroy() 
{
    if (rootView) 
    {
        rootView->destroy(); 
        delete rootView;
        rootView = nullptr;
    }
    
    // Clean up font resources
    if (globalAtlas) {
        cleanupFontRenderer(*globalAtlas);
        delete globalAtlas;
        globalAtlas = nullptr;
    }
}