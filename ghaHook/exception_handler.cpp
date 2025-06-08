#include <Windows.h>
#include <stdio.h>
#include <intrin.h>
#include "main.h"

static int _breakpointAddress = 0;
static int _debugRegister = -1;

static void decryptionFailure()
{
    ghaHook_fatal("HASP decryption failed, the game can't start!\n\nMake sure the correct dongle is connected properly.");
}
static void breakpointInstallFailure()
{
    ghaHook_fatal("Failed to install breakpoints, the game can't start!");
}
static int ghaHook_startup_func()
{
    init_ghaHook();
    return ((int(*)(void))_breakpointAddress)();
}

#define SET_BIT(p,n) ((p) |= (1 << (n)))
#define CLR_BIT(p,n) ((p) &= ~((1) << (n)))
static int set_exec_breakpoint(PCONTEXT ctx, DWORD address)
{
    int i = 0;
    DWORD *hwDbgRegisters = &ctx->Dr0;
    for (int i = 0; i < 4; i++)
    {
        if (hwDbgRegisters[i] == 0)
        {
            // set the debug register to our winmain entrypoint
            hwDbgRegisters[i] = address;
            // set Dr7 to enable a hw breakpoint
            SET_BIT(ctx->Dr7, 0 + (i * 2)); // enable Lx where x is the Dr
            CLR_BIT(ctx->Dr7, 1 + (i * 2)); // disable Gx where x is the Dr
            // set R/Wx to execution (00) where x is the Dr
            CLR_BIT(ctx->Dr7, 16 + (i * 4)); 
            CLR_BIT(ctx->Dr7, 17 + (i * 4));
            // set LENx to 00 where x is the Dr
            CLR_BIT(ctx->Dr7, 18 + (i * 4)); 
            CLR_BIT(ctx->Dr7, 19 + (i * 4));
            return i;
        }
    }
    return -1;
}
static void unset_exec_breakpoint(PCONTEXT ctx, int dbgReg)
{
    DWORD *hwDbgRegisters = &ctx->Dr0;
    hwDbgRegisters[dbgReg] = 0;
    // unset the breakpoint in Dr7
    SET_BIT(ctx->Dr7, 0 + (dbgReg * 2)); // enable Lx where x is the Dr
    CLR_BIT(ctx->Dr7, 1 + (dbgReg * 2)); // disable Gx where x is the Dr
}

static LONG __stdcall exception_handler(_EXCEPTION_POINTERS *ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x69696969)
    {
        // install our breakpoint
        _debugRegister = set_exec_breakpoint(ExceptionInfo->ContextRecord, _breakpointAddress);
        if (_debugRegister == -1)
        {
            ExceptionInfo->ContextRecord->Eip = (DWORD)breakpointInstallFailure;
        }
        // return to the exception handler install function
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else if (ExceptionInfo->ContextRecord->Eip == _breakpointAddress)
    {
        BYTE *startupFunc = (BYTE *)_breakpointAddress;
        // verify that the instructions decrypted correctly
        if (startupFunc[0] == 0xE8 && startupFunc[5] == 0xE8)
        {
            // set instruction pointer to our hook initialiser
            ExceptionInfo->ContextRecord->Eip = (DWORD)ghaHook_startup_func;
            // disable breakpoint
            unset_exec_breakpoint(ExceptionInfo->ContextRecord, _debugRegister);
        }
        else
        {
            // set instruction pointer to our decryption failure warning
            ExceptionInfo->ContextRecord->Eip = (DWORD)decryptionFailure;
        }
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    // fall back to the game/the kernel
    return EXCEPTION_CONTINUE_SEARCH;
}

void install_exception_handler(int breakpoint_address)
{
    _breakpointAddress = breakpoint_address;
    AddVectoredExceptionHandler(1, exception_handler);
    // raise an exception so we can install our breakpoint
    RaiseException(0x69696969, 0, 0, NULL);
}
