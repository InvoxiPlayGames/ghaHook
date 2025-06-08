#include <Windows.h>
#include <stdio.h>
#include <MinHook.h>

#include "exception_handler.h"
#include "gha_offsets.h"
#include "gha_jamma.h"
#include "ghaHook_version.h"
#include "config.h"

void ghaHook_fatal(const char *text)
{
    MessageBoxA(NULL, text, "ghaHook Fatal Error", MB_ICONERROR);
    exit(-1);
}

gha_exe_offsets offsets;
jammaop2_struct op2data;

int ghaHook_InputThreadActive = 1;
DWORD WINAPI ghaHook_InputThread(void *)
{
    while (ghaHook_InputThreadActive)
    {
        memset(&op2data, 0, sizeof(op2data));
        if (GetKeyState('Z') & 0x8000)
        {
            printf("start pressed\n");
            op2data.startP1 = 1;
            if (offsets.startWaitingCount1 != 0)
                *(int *)offsets.startWaitingCount1 = 1;
        }
        if (GetKeyState('X') & 0x8000)
        {
            printf("coin pressed\n");
            op2data.coin1 = 1;
            if (offsets.coinWaitingCount1 != 0)
                *(int *)offsets.coinWaitingCount1 = 1;
        }
        Sleep(30);
    }
    return 0;
}

int __stdcall _JUSB_Open_Hooked(void *handle)
{
    printf("JUSB_Open\n");
    return 0x41414141;
}

int __stdcall _JUSB_Close_Hooked(void *handle)
{
    printf("JUSB_Close\n");
    return 0;
}

int __stdcall _JUSB_GetReport_Hooked(void *handle, BYTE *report)
{
    printf("JUSB_GetReport\n");
    return 2;
}

int __stdcall _JUSB_SendReport_Hooked(void *handle, BYTE *report)
{
    printf("JUSB_SendReport %02x %02x %02x\n", report[0], report[1], report[2]);
    return 0;
}

int __cdecl JammaOpHooked(int opId, ...)
{
    int r = -9088; // "device not connected"
    va_list args;
    va_start(args, opId);
    if (opId != 0x2 && opId != 0x25)
    {
        printf("JammaOp: %02x\n", opId);
    }
    switch (opId)
    {
    case kOpReadDipSwitches:
        r = 0x4C; // no dip switches flipped
        break;
    case kOpReadData:
        {
            // read the keypad data out of our struct
            jammaop2_struct **out = va_arg(args, jammaop2_struct **);
            *out = &op2data;
            r = 0;
            break;
        }
    case kOpGetBoardRevString:
        {
            char *out_str = va_arg(args, char *);
            int str_length = va_arg(args, int);
            strncpy(out_str, "ghaHook " GHAHOOK_VERSION_STR, str_length);
            r = 0;
            break;
        }
    case kOpInitialize:
        r = 0; // no clue
        break;
    case kOpGetBoardRevInt1:
        r = 6;
        break;
    case kOpGetBoardRevInt2:
        r = 9;
        break;
    }
    va_end(args);
    return r;
}

bool __cdecl JammaIsBoardPresentHooked()
{
    return true;
}

int __stdcall hasp_login_hooked(int type, char * vendorcode, int *handle)
{
    // invalid parameter
    return 0x1c;
}

int __cdecl SIO_check_dongle_hooked()
{
    return 1;
}

// we should probably have this hook be optional, an arcade owner might Want periodic reboot 
void __cdecl SIO_update_periodicreboot_hooked()
{
    return;
}

void CodePatch(int address, void *data, int length)
{
    DWORD oldProtect, nope;
    VirtualProtect((void *)address, length, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void *)address, data, length);
    VirtualProtect((void *)address, length, oldProtect, &nope);
}

void init_console()
{
    AllocConsole();
    SetConsoleTitleA("ghaHook");
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
}

void init_ghaHook()
{
    init_console();
    load_config();
    printf("Hello from ghaHook " GHAHOOK_VERSION_STR "!\n");
    // hook all the functions
    MH_Initialize();
    if (config.EnableIOHooks)
    {
        MH_CreateHook((void *)offsets.JammaIsBoardPresent, JammaIsBoardPresentHooked, NULL);
        MH_CreateHook((void *)offsets.JammaOp, JammaOpHooked, NULL);
        MH_CreateHook((void *)offsets._JUSB_Open, _JUSB_Open_Hooked, NULL);
        MH_CreateHook((void *)offsets._JUSB_Close, _JUSB_Close_Hooked, NULL);
        MH_CreateHook((void *)offsets._JUSB_GetReport, _JUSB_GetReport_Hooked, NULL);
        MH_CreateHook((void *)offsets._JUSB_SendReport, _JUSB_SendReport_Hooked, NULL);
    }
    if (config.EnableDongleHooks)
    {
        MH_CreateHook((void *)offsets.hasp_login, hasp_login_hooked, NULL);
    }
    MH_CreateHook((void *)offsets.SIO_check_dongle, SIO_check_dongle_hooked, NULL);
    MH_CreateHook((void *)offsets.SIO_update_periodicreboot, SIO_update_periodicreboot_hooked, NULL);
    MH_EnableHook(MH_ALL_HOOKS);
    // start our thread
    CreateThread(NULL, 0x8000, ghaHook_InputThread, NULL, 0, NULL);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch(fdwReason) 
    { 
        case DLL_PROCESS_ATTACH:
            if (InitOffsets(&offsets) == -1)
            {
                ghaHook_fatal("Unsupported executable version!");
            }
            install_exception_handler(offsets.pre_main);
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

// fake DLL export, for injecting by modifying the executable. not recommended
extern "C" __declspec(dllexport) void ghaHook() {}
