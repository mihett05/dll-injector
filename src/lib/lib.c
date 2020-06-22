#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <TlHelp32.h>
#include <errno.h>


BOOL set_value(HANDLE hProc, char find_value, char need_value)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    DWORD dw_start = (DWORD)info.lpMinimumApplicationAddress;
    DWORD dw_max = (DWORD)info.lpMaximumApplicationAddress;
    
    DWORD last_address = dw_start;

    while (last_address < dw_max)
    {
        MEMORY_BASIC_INFORMATION mbi;
        VirtualQueryEx(hProc, last_address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

        if (mbi.State == MEM_COMMIT &&
            mbi.Protect != PAGE_READONLY &&
            mbi.Protect != PAGE_EXECUTE_READ &&
            mbi.Protect != PAGE_GUARD &&
            mbi.Protect != PAGE_NOACCESS
            )
        {
            char* buff = (char*)malloc(info.dwPageSize);
            if (ReadProcessMemory(hProc, last_address, buff, info.dwPageSize, NULL))
            {
                for (size_t i = 0; i < info.dwPageSize; i++)
                {
                    if (buff[i] == find_value)
                    {
                        char data = need_value;
                        WriteProcessMemory(hProc, last_address + i, &data, sizeof(char), NULL);
                    }
                }
            }
            free(buff);
        }
        last_address += info.dwPageSize;
    }
}

void attach()
{
    HANDLE hProc = GetCurrentProcess();
    if (set_value(hProc, 42, 90)) {
        printf("\nUpdated!\n");
    };
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
        attach();
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