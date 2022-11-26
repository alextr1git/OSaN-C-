#include "hDll.h"
#include "DrawYellowDll.h"
#include "DrawBlackDll.h"

namespace LabWork3
{
    hDll::hDll() 
    {
        this->dllNames = new std::vector<LPCWSTR>();
        this->dllNames->push_back(L"..\\..\\plugins\\DrawYellowDll.dll");
        this->dllNames->push_back(L"..\\..\\plugins\\DrawBlackDll.dll");
       

        this->loadedDll = LoadLibrary(this->dllNames->at(0));   

        if (loadedDll == NULL)
        {
            delete(this->dllNames);
            throw std::exception();
        }

        this->loadedDllIndex = 0;
        this->loadedProc = (drawProc)GetProcAddress(this->loadedDll, this->methName); 

        if (loadedProc == NULL)
        {
            delete(this->dllNames);
            FreeLibrary(this->loadedDll);
            throw std::exception();
        }
    }

    hDll::~hDll() //destructor
    {
        FreeLibrary(this->loadedDll);
        delete(this->dllNames);
    }

    const void hDll::draw(HDC hdc, int x, int y, int radius) //draw method
    {
        this->loadedProc(hdc, x, y, radius);
    }

    void hDll::switchDll()    //switch dll
    { 
        if (this->loadedDllIndex == this->dllNames->size() - 1) 
            this->loadedDllIndex = 0;
        else
            ++this->loadedDllIndex;

        FreeLibrary(this->loadedDll);
        this->loadedDll = LoadLibrary(this->dllNames->at(this->loadedDllIndex)); //load next
        this->loadedProc = (drawProc)GetProcAddress(this->loadedDll, this->methName); //paint
    }
}
