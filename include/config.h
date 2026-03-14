#pragma once
#include <wchar.h>

#define MAX_DEVICES 8
#define MAX_APPS 16
#define MAX_NAME 128

typedef struct {
    wchar_t target_apps[MAX_APPS][MAX_NAME];
    int target_app_count;
    wchar_t devices[MAX_DEVICES][MAX_NAME];
    int device_count;
    int poll_interval_ms;
    int autostart;
} Config;

extern Config g_config;

void load_config(const wchar_t* path);
