#ifndef ADDRESS_LIST_H
#define ADDRESS_LIST_H
#include <stdlib.h>
#include <windows.h>

typedef struct {
    DWORD list[4096];
    size_t size;
    DWORD count;
} address_list;

void find_addresses(HANDLE hProc, char value, address_list* list)
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
                    if (buff[i] == value)
                    {
                        list->list[list->count] = last_address + i;
                        list->count++;
                    }
                }
            }
            free(buff);
        }
        last_address += info.dwPageSize;
    }
}

BOOL set_value(HANDLE hProc, DWORD address, char value)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQueryEx(hProc, address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

    if (mbi.State == MEM_COMMIT &&
        mbi.Protect != PAGE_READONLY &&
        mbi.Protect != PAGE_EXECUTE_READ &&
        mbi.Protect != PAGE_GUARD &&
        mbi.Protect != PAGE_NOACCESS
        )
    {
        char data = value;
        return WriteProcessMemory(hProc, address, &data, sizeof(char), NULL);
    }

    return 0;
}

#endif