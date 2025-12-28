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

std::chrono::steady_clock::time_point startTime;
FontAtlas* globalAtlas = nullptr;  // Global persistent atlas

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

    // Test with size 32 (you confirmed this works)
    globalAtlas = initFont(53.5f);

    ui::ScrollView* scrollView = new ui::ScrollView();
    scrollView->setPosition(0, 110);
    scrollView->setSize(width, height - 320);
    scrollView->setColor(0.8f, 0.8f, 0.8f, 0.0f);
    scrollView->setContainerHeight(210 * 20 + 10);

    ui::View* topBar = new ui::View();
    topBar->setPosition(0, 0);
    topBar->setSize(width, 100);
    topBar->setColor(224.0f/255.0f, 25.0f/255.0f, 78.0f/255.0f, 1.0f);

    ui::TextView* title = new ui::TextView();
    title->setPosition(20, 5);
    title->setColor(1.0f, 1.0f, 1.0f, 0.0f);
    title->setColorText(1.0f, 1.0f, 1.0f, 1.0f);
    title->setSize(400, 100);
    title->setText("Owlette UI Demo");
    title->setFontAtlas(globalAtlas);
    topBar->addChild(title);

    rootView->addChild(topBar);

    for (int i = 0; i < 20; i++)
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

    ui::EditTextView* entry = new ui::EditTextView();
    entry->setPosition(0, height - 200);
    entry->setSize(width, 200);
    entry->setText("Hello!");
    entry->setColor(0.96f, 0.18f, 0.43f, 0.4f);
    entry->setColorText(0.0f, 0.0f, 0.0f, 1.0f);
    entry->setFontAtlas(globalAtlas);

    entry->setOnTouchDownListener([entry](float x, float y) {
        if (!entry->isFocused()) 
            entry->setFocused(true);
        else 
            entry->setFocused(false);
    });

    rootView->addChild(entry);
}

void onResize(int w, int h) 
{
    width = w;
    height = h;
    
    if (rootView)
    {
        rootView->setSize(width, height);
        rootView->setViewport(width, height);
        
        auto& children = rootView->getChildren();
        if (children.size() > 1) {
            children[1]->setSize(width, height - 320); // scrollView
        }
        if (children.size() > 2) {
            children[2]->setPosition(0, height - 200); // entry
            children[2]->setSize(width, 200);
        }
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