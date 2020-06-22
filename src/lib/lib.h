#ifndef LIB_H
#define LIB_H
#include <windows.h>
extern "C" __declspec(dllexport)

__declspec(dllexport)
BOOL APIENTRY DllMain(HINSTANCE hInst     /* Library instance handle. */ ,
                        DWORD reason        /* Reason this function is being called. */ ,
                        LPVOID reserved     /* Not used. */ );
#endif