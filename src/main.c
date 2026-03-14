#include <windows.h>
#include <stdio.h>
#include "config.h"
#include "worker.h"

#define AUTOSTART_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define AUTOSTART_VALUE L"AutoMuteApp"

static void configure_autostart(void)
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, AUTOSTART_KEY, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return;

    if (g_config.autostart) {
        wchar_t exe_path[MAX_PATH];
        if (GetModuleFileNameW(NULL, exe_path, MAX_PATH)) {
            RegSetValueExW(hKey, AUTOSTART_VALUE, 0, REG_SZ,
                (const BYTE*)exe_path, (DWORD)((wcslen(exe_path) + 1) * sizeof(wchar_t)));
        }
    } else {
        RegDeleteValueW(hKey, AUTOSTART_VALUE);
    }

    RegCloseKey(hKey);
}

int WINAPI WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPSTR lpCmdLine,
    int nShowCmd
) {
    (void)hPrevInst; 
    (void)lpCmdLine;
    (void)nShowCmd;

    load_config(L"config.json");
    configure_autostart();
    CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);
    Sleep(INFINITE);
}
