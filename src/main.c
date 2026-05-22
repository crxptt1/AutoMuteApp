#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include "config.h"
#include "worker.h"

#define AUTOSTART_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define AUTOSTART_VALUE L"AutoMuteApp"

static void get_config_path(wchar_t* buffer, size_t buffer_len)
{
    if (!buffer || buffer_len == 0) {
        return;
    }

    buffer[0] = L'\0';

    wchar_t exe_path[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, exe_path, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        wcsncpy(buffer, L"config.json", buffer_len - 1);
        buffer[buffer_len - 1] = L'\0';
        return;
    }

    wchar_t* last_slash = wcsrchr(exe_path, L'\\');
    if (!last_slash) {
        wcsncpy(buffer, L"config.json", buffer_len - 1);
        buffer[buffer_len - 1] = L'\0';
        return;
    }

    *(last_slash + 1) = L'\0';

    if (wcslen(exe_path) + wcslen(L"config.json") + 1 > buffer_len) {
        wcsncpy(buffer, L"config.json", buffer_len - 1);
        buffer[buffer_len - 1] = L'\0';
        return;
    }

    wcscpy(buffer, exe_path);
    wcscat(buffer, L"config.json");
}

static void configure_autostart(void)
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, AUTOSTART_KEY, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return;

    if (g_config.autostart) {
        wchar_t exe_path[MAX_PATH];
        DWORD len = GetModuleFileNameW(NULL, exe_path, MAX_PATH);
        if (len > 0 && len < MAX_PATH) {
            wchar_t quoted_path[MAX_PATH + 2];
            int written = swprintf(quoted_path, ARRAYSIZE(quoted_path), L"\"%ls\"", exe_path);
            if (written > 0 && written < (int)ARRAYSIZE(quoted_path)) {
                RegSetValueExW(hKey, AUTOSTART_VALUE, 0, REG_SZ,
                    (const BYTE*)quoted_path, (DWORD)((written + 1) * sizeof(wchar_t)));
            }
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

    wchar_t config_path[MAX_PATH];
    get_config_path(config_path, ARRAYSIZE(config_path));
    load_config(config_path);
    configure_autostart();
    CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);
    Sleep(INFINITE);
}
