#pragma once
#include "ui/View.h"

ui::View* getRootView();

void onInit();
void onResize(int width, int height);
void onDraw();
void onDestroy();