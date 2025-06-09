#include <Shlwapi.h>
#include "config.h"

ghaHook_config config;

void load_config()
{
    config.EnableIOHooks = 1;
    config.EnableDongleHooks = 1;
    config.EnableDeviceNameHook = 0;
    if (PathFileExistsA("disable_io_hooks.txt"))
        config.EnableIOHooks = 0;
    if (PathFileExistsA("disable_dongle_hooks.txt"))
        config.EnableDongleHooks = 0;
    if (PathFileExistsA("device_name_bypass.txt"))
        config.EnableDeviceNameHook = 1;
}
