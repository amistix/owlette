#include "main.h"
#include "ui/View.h"
#include "ui/ScrollView.h"
#include "ui/TextView.h"

ui::View* rootView = nullptr;
extern GLint width, height;

ui::View* getRootView() {return rootView;}

void onInit() 
{
    if (rootView) return;

    rootView = new ui::View();
    

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

        placeholder->addChild(content);
        scrollView->addChild(placeholder);

        placeholder->setOnTouchDownListener([placeholder, scrollView, content](float x, float y){
            scrollView->focus(x, y);
            // content->setFocused(true);
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

void onResize(int width, int height) 
{
    if (rootView)
    {
        rootView->setSize(width, height);
        rootView->setViewport(width, height);
    }
}

void onDraw() 
{
    if (rootView) 
    {
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
}