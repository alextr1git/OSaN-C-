#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include "hDll.h"
#include "DrawBlackDll.h"
//#include "DrawYellowDll.h"

#define RADIUS 70

struct Point
{
    int x;
    int y;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void hPaint(HWND hwnd);

LabWork3::hDll* hDll = NULL;
Point* point = NULL;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"LabWork3";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"LabWork3", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return EXIT_FAILURE;

    hDll = new LabWork3::hDll();

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete(hDll);
    if (point != NULL)
        delete(point);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_PAINT:
            hPaint(hwnd);
            break;

        case WM_LBUTTONUP:
            point = new Point();
            point->x = GET_X_LPARAM(lParam);
            point->y = GET_Y_LPARAM(lParam);
            InvalidateRect(hwnd, NULL, true);
            break;

        case WM_KEYDOWN:
            if (GetKeyState(VK_SHIFT)) {
                hDll->switchDll();
               
            }
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void hPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    if (point != NULL)
    {
       // dllHandler->draw(hdc, point->x, point->y, RADIUS);
        draw(hdc, point->x, point->y, RADIUS);
        delete(point);
        point = NULL;
    }

    EndPaint(hwnd, &ps);
}


