#pragma once
#include <jni.h>
#include "ui/FontRenderer.h"
#include "ui/FontAtlas.h"
#include <GLES2/gl2.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <cstring>

FontAtlas* initFont(float textSize);