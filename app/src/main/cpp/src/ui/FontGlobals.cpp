#include <vector>
#include <string>

// DEFINING the globals (storage)

int g_fontWidth = 0;
int g_fontHeight = 0;

unsigned char* g_fontPixels = nullptr;

std::vector<int> g_glyphX;
std::vector<int> g_glyphW;

std::string g_charset;
