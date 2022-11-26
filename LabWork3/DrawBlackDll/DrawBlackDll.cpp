#define BUILD_DLL

#include <math.h>
#include "DrawBlackDll.h"

namespace add
{
    extern "C" __declspec(dllexport) void draw(HDC hdc, int x, int y, int radius)
    {
        HBRUSH brush = CreateSolidBrush(RGB(20, 20, 20));
        HPEN pen = CreatePen(PS_SOLID, 5, RGB(110, 110, 80));

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);

        const int VARIANT = 5;
        double angle = 2 * 3.14159265358979323846 / VARIANT;

        POINT* points = new POINT[VARIANT];
        for (int i = 0; i < VARIANT; ++i)
        {
            points[i].x = x + radius * sin(i * angle);
            points[i].y = y + radius * cos(i * angle);
        }

        Polygon(hdc, points, VARIANT);
        delete(points);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(brush);
        DeleteObject(pen);
    }
}