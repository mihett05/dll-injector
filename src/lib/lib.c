#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <TlHelp32.h>
#include <errno.h>
#include "address_list.h"

void attach(HINSTANCE hInst)
{
    HANDLE hProc = GetCurrentProcess();

    address_list list;
    list.count = 0;

    find_addresses(hProc, 42, &list);
    for (size_t i = 0; i < list.count; i++)
    {
        set_value(hProc, list.list[i], 90);
    }
    CloseHandle(hProc);
}

__declspec(dllexport)
BOOL APIENTRY DllMain(HINSTANCE hInst     /* Library instance handle. */ ,
                        DWORD reason        /* Reason this function is being called. */ ,
                        LPVOID reserved     /* Not used. */ )
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
        attach(hInst);
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}