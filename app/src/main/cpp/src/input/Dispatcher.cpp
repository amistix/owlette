#include "input/Dispatcher.h"

namespace input {


void setRoot(ui::View* v) {root = v;}

void handleTouch(int action, float x, float y) {
    if (!root) return;
    vec2<float> pos = {x, y};

    switch (action) 
    {
        case 0: // ACTION_DOWN
        {
            ui::View* target = root->hitTest(pos);
            activeView = target;
            if (target) target->onTouchDown(pos);
            break;
        }
        case 2: // ACTION_MOVE
        {
            if (activeView) activeView->onTouchMove(pos);
            break;
        }
        case 1: // ACTION_UP
        {
            if (activeView) 
            {
                activeView->onTouchUp(pos);
                activeView = nullptr;
            }
            break;
        }
    }
}
}