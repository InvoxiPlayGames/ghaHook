#include <memory.h>
#include "gha_offsets.h"

// Wed Dec 09 12:13:42 2009 build of GH3.exe
void InitOffsets_4b1fe8d6(gha_exe_offsets *offsets)
{
    // core
    offsets->WinMainCRTStartup = 0x6a778e;
    offsets->pre_main = 0x401530;
    // function offsets
    offsets->SIO_check_dongle = 0x46b1b0;
    offsets->SIO_update_periodicreboot = 0x46b0a0;
    offsets->JammaIsBoardPresent = 0x48f060;
    offsets->JammaOp = 0x48fe40;
    offsets->_JUSB_Open = 0x5d0520;
    offsets->_JUSB_Close = 0x5d0720;
    offsets->_JUSB_GetReport = 0x5d0460;
    offsets->_JUSB_SendReport = 0x5d07e0;
    offsets->hasp_login = 0x800490;
    offsets->hasp_logout = 0x800520;
    offsets->hasp_read = 0x8012a0;
    offsets->hasp_write = 0x801370;
    offsets->hasp_get_sessioninfo = 0x801010;
    offsets->RTInitCoinUp = 0x511e00;
    offsets->RTCoinUpSetLocation = 0x511cd0;
    // instruction offsets
    offsets->SIO_Device_get_status_name_check = 0x5194f9; // jnz after the name check in SIO::Device::get_status
    // rdata
    offsets->version_txt_path = 0x929858;
    // waiting counts for service panel buttons
    offsets->testWaitingCount = 0xc26c40;
    offsets->startWaitingCount1 = 0xc26bec;
    offsets->startWaitingCount2 = 0xc26bf4;
    offsets->coinWaitingCount1 = 0xc26c04;
    offsets->coinWaitingCount2 = 0xc26c0c;
    offsets->coinWaitingCount3 = 0xc26c34;
    offsets->coinWaitingCount4 = 0xc26c48;
    offsets->volumeWaitingCount1 = 0xc26c1c;
    offsets->volumeWaitingCount2 = 0xc26c24;
}

int InitOffsets(gha_exe_offsets *offsets)
{
    memset(offsets, 0, sizeof(gha_exe_offsets));
    // switch IMAGE_NT_HEADERS32->FileHeaders->TimeDateStamp
    switch (*(int *)0x400158)
    {
    case 0x4b1fe8d6:
        InitOffsets_4b1fe8d6(offsets);
        return 0;
    }
    return -1;
}
