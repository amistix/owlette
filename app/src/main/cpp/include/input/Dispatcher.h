#pragma once
#include "ui/View.h"

namespace input 
{
    static ui::View* root = nullptr;
    static ui::View* activeView = nullptr;

    void setRoot(ui::View* v);
    void handleTouch(int action, float x, float y);
}