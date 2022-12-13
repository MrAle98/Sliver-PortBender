// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"

#include <Windows.h>
#include "Sliver-PortBender.h"

#pragma warning(disable : 4996)
extern "C" {

    HINSTANCE hInst = nullptr;
    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
    {
        if (hInst == nullptr) { hInst = hinstDLL; }
        switch (fdwReason)
        {
        case DLL_PROCESS_ATTACH:
        {
            break;
        }
        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
        }

        return TRUE;
    }
}
