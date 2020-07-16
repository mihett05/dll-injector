#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <TlHelp32.h>

HANDLE hCon;
int width, height;

COORD get_cursor_coord()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(hCon, &info))
    {
        return info.dwCursorPosition;
    }
    else
    {
        COORD coord = {0, 0};
        return coord;
    }
}

void clear_list(int start_y, int count)
{
    COORD coord = {0, start_y};

    for (size_t i = 0; i < count; i++)
    {
        coord.Y = start_y + i;
        SetConsoleCursorPosition(hCon, coord);
        putch('\r');
        for (size_t j = 0; j < width; j++)
        {
            putch('\0');
        }
    }

    coord.Y = start_y;
    SetConsoleCursorPosition(hCon, coord);
}

int get_pid(char* process_name)
{
    BOOL refresh = 0;
    COORD coord = get_cursor_coord();
    coord.Y = coord.Y + 3;

    while (TRUE)
    {
        int pids[256];
        int pids_count = 0;
        int selected_pid_index = 0;

        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 peProcessEntry;

        peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
        Process32First(hSnapshot, &peProcessEntry);
        do
        {
            if (strcmp(process_name, peProcessEntry.szExeFile) == 0 && peProcessEntry.th32ProcessID > 0)
            {
                pids[pids_count] = peProcessEntry.th32ProcessID;
                pids_count++;
            }
        } while(Process32Next(hSnapshot, &peProcessEntry));
        CloseHandle(hSnapshot);

        if (pids_count > 1)
        {
            if (!refresh)
            {
                printf("Press SPACE to refresh list\n");
                printf("Press ESC to leave\n");
                printf("Select proccess: (ENTER)\n");
            }
            
            for (size_t i = 0; i < pids_count; i++)
            {
                printf("(%c) %d %s\n", selected_pid_index == i? '*' : ' ', pids[i], process_name);
            }

            refresh = 0;
            while (!refresh)
            {
                COORD tmp = {0, coord.Y + selected_pid_index};
                switch (getch())
                {
                case 72: // UP
                    SetConsoleCursorPosition(hCon, tmp);
                    printf("\r( ");
                    if (selected_pid_index == 0)
                    {
                        selected_pid_index = pids_count - 1;
                    }
                    else
                    {
                        selected_pid_index--;
                    }
                    break;
                case 80: // DOWN
                    SetConsoleCursorPosition(hCon, tmp);
                    printf("\r( ");
                    if (selected_pid_index == pids_count - 1)
                    {
                        selected_pid_index = 0;
                    }
                    else
                    {
                        selected_pid_index++;
                    }
                    break;
                case VK_SPACE:
                    refresh = 1;
                    clear_list(coord.Y, pids_count);
                    break;
                case VK_RETURN:
                    clear_list(coord.Y - 3, pids_count + 3);
                    return pids[selected_pid_index];
                case VK_ESCAPE:
                    clear_list(coord.Y - 3, pids_count + 3);
                    return 0;
                default:
                    break;
                }
                if (!refresh)
                {
                    Sleep(40);
                    tmp.Y = coord.Y + selected_pid_index;
                    SetConsoleCursorPosition(hCon, tmp);
                    printf("\r(*");
                }
            }
        }
        else if (pids_count == 1)
        {
            return pids[0];
        }
        else
        {
            printf("Not found any processes with name \"%s\"", process_name);
            return 0;
        }
    }
}

BOOL inject(int pid, char* dll_name)
{
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    if (hProc)
    {
        LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        LPVOID dereercomp = VirtualAllocEx(hProc, NULL, strlen(dll_name), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        WriteProcessMemory(hProc, dereercomp, dll_name, strlen(dll_name), NULL);
        HANDLE asdc = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
        if (asdc)
        {
            WaitForSingleObject(asdc, INFINITE);
            VirtualFreeEx(hProc, dereercomp, strlen(dll_name), MEM_RELEASE);
            CloseHandle(asdc);
            CloseHandle(hProc);
            return 1;
        }
        CloseHandle(asdc);
        CloseHandle(hProc);
        
    }
    return 0;
}


int main(int argc, char const* argv[])
{
    hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hCon, &info);

    width = info.srWindow.Right - info.srWindow.Left + 1;
    height = info.srWindow.Bottom - info.srWindow.Top + 1;

    char process_name[1024];
    if (argc >= 2)
    {
        strcpy(process_name, argv[1]);
    }
    else
    {
        printf("Name: ");
        scanf("%s", process_name);
    }

    int pid = get_pid(process_name);
    if (pid)
    {
        printf("Pid: %d\n", pid);
        char dll_name[1024];
        if (argc >= 3)
        {
            strcpy(dll_name, argv[2]);
            printf("DLL: %s\n", dll_name);
        }
        else
        {
            printf("DLL: ");
            scanf("%s", dll_name);
        }
        _Bool status = inject(pid, dll_name);
        if (status)
        {
            printf("Injected");
        }
        else
        {
            printf("Can't inject dll");
        }
        
    }

    CloseHandle(hCon);
    return 0;
}

