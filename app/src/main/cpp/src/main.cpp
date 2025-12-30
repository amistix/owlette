#include "main.h"
#include "ui/TextView.h"

#include "ui/FontRenderer.h"
#include "ui/Font_JNI.h"

#include "animation/Animator.h"
#include <math.h>

#define PI 3.14159265359

ui::View* rootView = nullptr;
extern GLint width, height;

FontAtlas* globalAtlas = nullptr;

ui::View* getRootView() { return rootView; }

void onInit() 
{
    if (rootView) 
    {
        if (globalAtlas && globalAtlas->pixels) 
        {
            globalAtlas->fontTexture = 0;
            uploadFontTexture(*globalAtlas);
        }
        return;
    }

    rootView = new ui::View();

    rootView->setViewport(vec2<float>{width, height});
    rootView->setPosition(vec2<float>{0,0});
    rootView->setColor(vec4<float>{0.9f, 0.9f, 0.9f, 1.0f});
    rootView->setSize(vec2<float>{width, height});

    globalAtlas = initFont(30.0f);

    ui::TextView* greatingsText = new ui::TextView();
    greatingsText->setPosition(vec2<float>(0, 0));
    greatingsText->setSize(vec2<float>{width - 40, 60});
    greatingsText->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    greatingsText->setFontAtlas(globalAtlas);

    greatingsText->setColor(vec4<float>{0.0f, 0.0f, 0.0f, 0.0f});
    greatingsText->setColorText(vec4<float>{0.0f, 0.0f, 0.0f, 1.0f});

    animation::Animator::animatePosition(greatingsText,
        vec2<float>(0, 0), vec2<float>(20, height / 2 - 30),
        [](float t)
        {
            double k = 10.0;  // steepness (change if needed)
            return (tanh(k * (t - 0.5)) + tanh(k / 2.0)) / (2.0 * tanh(k / 2.0));
        }, 3.0f);

    rootView->addChild(greatingsText);
}

void onResize(int w, int h) 
{
    vec2<float> size = {w, h};
    
    if (rootView)
    {
        rootView->setSize(size);
        rootView->setViewport(size);
    }
}

void onDraw() 
{
    if (rootView) rootView->draw();
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
    if (globalAtlas) 
    {
        cleanupFontRenderer(*globalAtlas);
        delete globalAtlas;
        globalAtlas = nullptr;
    }
}