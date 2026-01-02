#include "main.h"

ui::View* rootView = nullptr;
extern GLint width, height;

extern FontAtlas* globalAtlas;
extern FontAtlas* largeAtlas;
extern FontAtlas* bigAtlas;
extern FontAtlas* mediumAtlas;
extern FontAtlas* littleAtlas;
extern FontAtlas* tinyAtlas;


ui::View* getRootView() { return rootView; }
void attachMessage(const std::string& authorName, const std::string& text, bool ownMessage);

void onInit() 
{
    if (rootView) 
    {
        std::vector<FontAtlas*> atlases = {
        globalAtlas, largeAtlas, bigAtlas, mediumAtlas, littleAtlas, tinyAtlas
        };
        for (FontAtlas* atlas : atlases)
        {
            if (atlas && atlas->pixels) 
            {
                atlas->fontTexture = 0;
                uploadFontTexture(*atlas);
            }
        }
        return;
    }

    rootView = new ui::View();

    rootView->setViewport(vec2<float>{width, height});
    rootView->setPosition(vec2<float>{0,0});
    rootView->setColor(vec4<float>{1.0f, 1.0f, 1.0f, 1.0f});
    rootView->setSize(vec2<float>{width, height});

    globalAtlas = initFont(height * 0.02f);
    largeAtlas = initFont(height * 0.05f);
    bigAtlas = initFont(height * 0.03f);
    mediumAtlas = initFont(height * 0.02f);
    littleAtlas = initFont(height * 0.015f);
    tinyAtlas = initFont(height * 0.01f);
    

    ui::View* topBar = new ui::View();
    topBar->setPosition(vec2<float>(0.0f, 0.0f));
    topBar->setSize(vec2<float>{width * 1.0f, height * 0.065f});
    topBar->setColor(vec4<float>(224.0f/255.0f, 25.0f/255.0f, 78.0f/255.0f, 1.0f));
    
    ui::TextView* title = new ui::TextView();
    title->setPosition(vec2<float>(height * 0.02f, height * 0.005f));
    title->setColor(vec4<float>(1.0f, 1.0f, 1.0f, 0.0f));
    title->setColorText(vec4<float>(1.0f, 1.0f, 1.0f, 1.0f));
    title->setSize(vec2<float>(width * 0.7f, height * 0.05f));
    title->setText("Owlette");
    title->setFontAtlas(largeAtlas);

    topBar->addChild(title);
    rootView->addChild(topBar);

    ui::ScrollView* chatScrollView = new ui::ScrollView();
    chatScrollView->setPosition(vec2<float>(vec2<float>{0.0f, height * 0.065f}));
    chatScrollView->setSize(vec2<float>{width * 1.0f, height * 0.87f});
    chatScrollView->setColor(vec4<float>(0.5f, 0.5f, 1.0f, 0.5f));
    chatScrollView->setContainerHeight(0.0f);

    chatScrollView->setOnTouchDownListener([chatScrollView](vec2<float> eventPosition) {
        chatScrollView->focus(eventPosition);
    });
    chatScrollView->setOnTouchMoveListener([chatScrollView](vec2<float> eventPosition) {
        chatScrollView->scroll(eventPosition);
    });
    chatScrollView->setOnTouchUpListener([chatScrollView](vec2<float> eventPosition) {
        chatScrollView->unfocus(eventPosition);
    });

    rootView->addChild(chatScrollView);

    ui::View* entryBar = new ui::View();
    entryBar->setPosition(vec2<float>(0.0f, height * 0.935f));
    entryBar->setSize(vec2<float>{width * 1.0f, height * 0.065f});
    entryBar->setColor(vec4<float>(0.9f, 0.9f, 0.9f, 1.0f));

    
    ui::View* entryButtonSend = new ui::View();
    entryButtonSend->setPosition(vec2<float>(width - height * 0.065f, 0.0f));
    entryButtonSend->setSize(vec2<float>{height * 0.065f, height * 0.065f});
    entryButtonSend->setColor(vec4<float>(224.0f/255.0f, 25.0f/255.0f, 78.0f/255.0f, 1.0f));
    
    ui::TextView* entryButtonSendIcon = new ui::TextView();
    entryButtonSendIcon->setHittable(false);
    entryButtonSendIcon->setText(">");
    entryButtonSendIcon->setFontAtlas(bigAtlas);
    entryButtonSendIcon->setColorText(vec4<float>(1.0f, 1.0f, 1.0f, 1.0f));
    entryButtonSendIcon->setColor(vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
    
    TextMetrics metricsEntryIcon = measureTextWrapped(
        entryButtonSendIcon->getText(),
        width,  
        *(entryButtonSendIcon->getFontAtlas())
    );
    
    entryButtonSendIcon->setSize(vec2<float>(metricsEntryIcon.width, metricsEntryIcon.height));
    entryButtonSendIcon->setPosition(vec2<float>((height * 0.065f - metricsEntryIcon.width) / 2, (height * 0.065f - metricsEntryIcon.height) / 2));
    
    ui::EditTextView* entry = new ui::EditTextView();
    entry->setColor(vec4<float>(1.0f, 1.0f, 1.0f, 0.0f));
    entry->setColorText(vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    entry->setSize(vec2<float>(width * 1.0f - 60.0f, height * 0.065f - 60.0f));
    entry->clearText();
    entry->setHittable(false);
    entry->setFontAtlas(mediumAtlas);
    
    TextMetrics metricsEntry = measureTextWrapped(
        (entry->getText().empty()) ? "Type here" : entry->getText(),
        width - height * 0.065f,  
        *(entry->getFontAtlas())
    );

    for (int i = 0; i < 10; i++)
    {
        attachMessage("theownerofthisworld", "hello", false);
    }
    
    entry->setPosition(vec2<float>((height * 0.065f - metricsEntry.height) / 2, (height * 0.065f - metricsEntry.height) / 2));
    
    entryBar->setOnTouchDownListener([entry](vec2<float> eventPosition)
    {
        entry->setFocused(true);
    });
    
    entryButtonSend->setOnTouchDownListener([entryButtonSend](vec2<float> eventPosition)
    {
        entryButtonSend->setColor(vec4<float>(199.0f/255.0f, 0.0f/255.0f, 53.0f/255.0f, 1.0f));
    });
    
    entryButtonSend->setOnTouchUpListener([entryButtonSend, entry](vec2<float> eventPosition)
    {
        attachMessage("You", entry->getText(), true);
        entry->clearText();
        entryButtonSend->setColor(vec4<float>(224.0f/255.0f, 25.0f/255.0f, 78.0f/255.0f, 1.0f));
    });
    
    entryBar->addChild(entry);
    entryButtonSend->addChild(entryButtonSendIcon);
    entryBar->addChild(entryButtonSend);
    rootView->addChild(entryBar);
    
    // ui::TextView* greatingsText = new ui::TextView();
    // greatingsText->setPosition(vec2<float>(20, 0));
    // greatingsText->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    // greatingsText->setFontAtlas(mediumAtlas);
    
    // TextMetrics metrics = measureTextWrapped(
        //     greatingsText->getText(),
        //     width - 40.0f,  
        //     *(greatingsText->getFontAtlas())
        // );
        
        // greatingsText->setSize(vec2<float>(width - 40.0f, metrics.height));
    
    // greatingsText->setColor(vec4<float>{0.0f, 0.0f, 0.0f, 0.0f});
    // greatingsText->setColorText(vec4<float>{0.0f, 0.0f, 0.0f, 1.0f});

    // animation::Animator::animatePosition(greatingsText,
    //     vec2<float>(20, 0), vec2<float>(20, height / 2 - 30),
    //     [](float t)
    //     {
    //         double k = 10.0;  // steepness (change if needed)
    //         return (tanh(k * (t - 0.5)) + tanh(k / 2.0)) / (2.0 * tanh(k / 2.0));
    //     }, 3.0f
    // );

    // animation::Animator::animateColor(greatingsText,
    //     vec4<float>(0.0f, 0.0f, 0.0f, 0.0f), vec4<float>(0.0f, 0.0f, 0.0f, 0.5f),
    //     [](float t)
    //     {
    //         double k = 10.0;  // steepness (change if needed)
    //         return (tanh(k * (t - 0.5)) + tanh(k / 2.0)) / (2.0 * tanh(k / 2.0));
    //     }, 3.0f
    // );

    // rootView->addChild(greatingsText);
}

void attachMessage(const std::string& authorName, const std::string& text, bool ownMessage)
{
    if (!rootView) return;
    
    auto& children = rootView->getChildren();
    if (children.size() < 2) return;
    
    ui::ScrollView* chatScrollView = static_cast<ui::ScrollView*>(children[1]);
    if (!chatScrollView) return;

    float maxMessageWidth = width * 0.8f;
    

    ui::View* messageBar = new ui::View();
    messageBar->setColor(vec4<float>(0.9f, 0.9f, 0.9f, 1.0f));
    

    ui::TextView* authorNameView = new ui::TextView();
    
    std::string displayName = authorName;
    if (displayName.length() > 10) {
        displayName = displayName.substr(0, 10) + "...";
    }
    
    authorNameView->setText(displayName);
    authorNameView->setHittable(false);
    authorNameView->setFontAtlas(mediumAtlas);
    authorNameView->setColorText(vec4<float>(224.0f/255.0f, 25.0f/255.0f, 78.0f/255.0f, 1.0f));
    authorNameView->setColor(vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
    

    TextMetrics metricsAuthorName = measureTextWrapped(
        authorNameView->getText(),
        maxMessageWidth - 20.0f,
        *mediumAtlas
    );
    
    authorNameView->setSize(vec2<float>(metricsAuthorName.width, metricsAuthorName.height));
    
    ui::TextView* textView = new ui::TextView();
    textView->setText(text);
    textView->setHittable(false);
    textView->setPosition(vec2<float>(10.0f, 10.0f + metricsAuthorName.height + 5.0f)); 
    textView->setFontAtlas(mediumAtlas);
    textView->setColorText(vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    textView->setColor(vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
    
    TextMetrics metricsText = measureTextWrapped(
        textView->getText(),
        maxMessageWidth - 20.0f,
        *mediumAtlas
    );
    
    textView->setSize(vec2<float>(metricsText.width, metricsText.height));
    
    float contentWidth = std::max(metricsText.width, metricsAuthorName.width);
    float messageBarWidth = contentWidth + 20.0f;
    messageBarWidth = std::max(messageBarWidth, 100.0f);
    
    float totalHeight = 10.0f + metricsAuthorName.height + 5.0f + metricsText.height + 10.0f;
    messageBar->setSize(vec2<float>(messageBarWidth, totalHeight));
    
    float currentContentHeight = chatScrollView->getContainerHeight();
    
    if (ownMessage) {
        // Right-aligned for own messages
        messageBar->setPosition(vec2<float>(width * 0.95f - messageBarWidth, currentContentHeight + 10.0f));
        authorNameView->setPosition(vec2<float>(messageBarWidth - metricsAuthorName.width - 10.0f, 10.0f));
    } else {
        // Left-aligned for other messages
        messageBar->setPosition(vec2<float>(width * 0.05f, currentContentHeight + 10.0f));
        authorNameView->setPosition(vec2<float>(10.0f, 10.0f));
    }
    
    messageBar->setOnTouchDownListener([chatScrollView](vec2<float> eventPosition) {
        chatScrollView->onTouchDown(eventPosition);
    });
    messageBar->setOnTouchMoveListener([chatScrollView](vec2<float> eventPosition) {
        chatScrollView->onTouchMove(eventPosition);
    });
    messageBar->setOnTouchUpListener([chatScrollView](vec2<float> eventPosition) {
        chatScrollView->onTouchUp(eventPosition);
    });
    
    messageBar->addChild(authorNameView);
    messageBar->addChild(textView);
    chatScrollView->addChild(messageBar);

    float scrollValue = chatScrollView->getScrollValue();
    chatScrollView->setContainerHeight(currentContentHeight + totalHeight + 20.0f);

    if(scrollValue > 0.999f) 
    {
        animation::Animator::animateCustom<float>(
            [chatScrollView](float value) {
                chatScrollView->setScrollValue(value);
            },
            chatScrollView->getScrollValue(),
            1.0f,
            [](float t) {
                return t * t;
            },
            0.1f
        );
    }

}

void onKeyboardOpen(float visibleHeight)
{
    if (rootView)
    {
        ui::View* entryBar = rootView->getChildren()[2];
        ui::ScrollView* chatScrollView = static_cast<ui::ScrollView*>(rootView->getChildren()[1]);
        entryBar->setPosition(vec2<float>{0.0f, visibleHeight - height * 0.065f});
        float oldChatWidth = chatScrollView->getSize().x;
        float a = chatScrollView->getScrollValue();
        chatScrollView->setSize(vec2<float>{oldChatWidth, visibleHeight - height * 0.13f});
        chatScrollView->setScrollValue(a);
    }
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

    std::vector<FontAtlas*> atlases = {
        globalAtlas, largeAtlas, bigAtlas, mediumAtlas, littleAtlas, tinyAtlas
    };
    for (FontAtlas* atlas : atlases)
    {
        if (atlas) 
        {
            cleanupFontRenderer(*atlas);
            delete atlas;
            atlas = nullptr;
        }
    }

    atlases.clear();
}

