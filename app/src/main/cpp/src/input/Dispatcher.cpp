#include "input/Dispatcher.h"

namespace input {


void setRoot(ui::View* v) {
    root = v;
}

void handleTouch(int action, float x, float y) {
    if (!root) return;

    switch (action) {
        case 0: // ACTION_DOWN
        {
            ui::View* target = root->hitTest(x, y);
            activeView = target;
            if (target) target->onTouchDown(x, y);
            break;
        }
        case 2: // ACTION_MOVE
        {
            if (activeView) {
                activeView->onTouchMove(x, y);
            }
            break;
        }
        case 1: // ACTION_UP
        {
            if (activeView) {
                activeView->onTouchUp(x, y);
                activeView = nullptr;
            }
            break;
        }
    }
}
}