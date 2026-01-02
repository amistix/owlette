#pragma once
#include "ui/View.h"
#include "ui/TextView.h"
#include "ui/EditTextView.h"
#include "ui/ScrollView.h"

#include "ui/FontRenderer.h"
#include "ui/Font_JNI.h"

#include "animation/Animator.h"
#include <math.h>

ui::View* getRootView();

void onInit();
void onResize(int width, int height);
void onKeyboardOpen(float visibleHeight);
void onDraw();
void onDestroy();