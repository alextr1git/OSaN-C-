#ifndef DRAWING_DLL
#define DRAWING_DLL

#include <windows.h>

#ifdef BUILD_DLL
#define DLL_TYPE __declspec(dllexport)
#else
#define DLL_TYPE __declspec(dllimport)
#endif

extern "C" DLL_TYPE void draw(HDC hdc, int x, int y, int radius);

#endif
