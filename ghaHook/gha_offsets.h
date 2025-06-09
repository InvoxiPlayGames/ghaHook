#ifndef GHA_OFFSETS_H
#define GHA_OFFSETS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _gha_exe_offsets
{
    // core
    int WinMainCRTStartup;
    int pre_main;
    // functions
    int SIO_check_dongle; // SIO::check_dongle
    int SIO_update_periodicreboot; // SIO::update_periodicreboot
    int JammaIsBoardPresent;
    int JammaOp;
    int _JUSB_Open;
    int _JUSB_Close;
    int _JUSB_GetReport;
    int _JUSB_SendReport;
    int hasp_login;
    int hasp_logout;
    int hasp_read;
    int hasp_write;
    int hasp_get_sessioninfo;
    int RTInitCoinUp;
    int RTCoinUpSetLocation;
    // instruction patches
    int SIO_Device_get_status_name_check;
    // rdata
    int version_txt_path;
    // globals
    int testWaitingCount; // ScriptCheckTestWaitingCount
    int startWaitingCount1; // ScriptCheckStartWaitingCount
    int startWaitingCount2;
    int coinWaitingCount1; // ScriptCheckCoinWaitingCount
    int coinWaitingCount2;
    int coinWaitingCount3;
    int coinWaitingCount4;
    int volumeWaitingCount1; // ScriptCheckVolumeWaitingCount
    int volumeWaitingCount2;
} gha_exe_offsets;

void InitOffsets_4b1fe8d6(gha_exe_offsets *offsets);
int InitOffsets(gha_exe_offsets *offsets);

#ifdef __cplusplus
}
#endif

#endif // GHA_OFFSETS_H
