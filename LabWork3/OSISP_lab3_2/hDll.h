#ifndef DLLHANDLER_H
#define DLLHANDLER_H

#include <wtypes.h>
#include <vector>

namespace LabWork3
{
    typedef void (*drawProc)(HDC hdc, int x, int y, int radius); 

    class hDll
    {
        public:
        hDll();
        ~hDll();

        const void draw(HDC hdc, int x, int y, int radius);
        void switchDll();

        private:
        const LPCSTR methName = "draw";

        std::vector<LPCWSTR>* dllNames;
        drawProc loadedProc;
        HMODULE loadedDll;
        int loadedDllIndex;
    };
}

#endif
